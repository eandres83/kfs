#include "socket.h"

// TODO: Implement bind, socket and connect function to better manage about the socket

static int32_t put_file(struct file *new_file, proc_t *process)
{
	for (int i = 0; i < MAX_FD; i++)
	{
		if (process->fd_table[i] == NULL)
		{
			process->fd_table[i] = new_file;
			return (i);
		}
	}
	return (-1);
}

size_t	socket_close(struct vfs_node *node)
{
	struct socket_channel *socket = (struct socket_channel *)node->fs_info;

	socket->fds_count--;
	if (socket->fds_count == 0)
		kfree(node->fs_info);
	kfree(node);
	return (0);
}

static struct ops socket_ops = {
	.read = NULL,
	.write = NULL,
	.open = NULL,
	.close = socket_close,
	.readdir = NULL,
	.finddir = NULL,
};

ssize_t socketpair(int domain, int type, int protocol, int *sv)
{
	proc_t *process = get_current_process();
	int kernel_socket[2];

	if (domain != 1 || type != 2 || protocol != 0)
		return (-1);

	struct socket_channel *socket = kmalloc(sizeof(struct socket_channel));
	if (!socket)
		return (-1);
	kmemset(socket, 0, sizeof(struct socket_channel));
	socket->fds_count = 2;

	struct vfs_node *node_ab = kmalloc(sizeof(struct vfs_node));
	if (!node_ab)
		return (kfree(socket), -1);
	kmemset(node_ab, 0, sizeof(struct vfs_node));
	node_ab->ops = &socket_ops;
	node_ab->fs_info = socket;

	struct vfs_node *node_ba = kmalloc(sizeof(struct vfs_node));
	if (!node_ba)
		return (kfree(node_ab), kfree(socket), -1);
	kmemset(node_ba, 0, sizeof(struct vfs_node));
	node_ba->ops = &socket_ops;
	node_ba->fs_info = socket;

	struct file *file_a = file_allocate();
	if (!file_a)
		return (kfree(node_ab), kfree(socket), kfree(node_ba), -1);
	file_a->node = node_ab;
	file_a->flags = O_SOCKET_SIDE_A;

	struct file *file_b = file_allocate();
	if (!file_b)
		return (kmemset(file_a, 0, sizeof(struct file)), kfree(node_ab), kfree(socket), kfree(node_ba), -1);
	file_b->node = node_ba;
	file_b->flags = O_SOCKET_SIDE_B;

	kernel_socket[0] = put_file(file_a, process);
	if (kernel_socket[0] == -1)
	{
		kmemset(file_a, 0, sizeof(struct file));
		kmemset(file_b, 0, sizeof(struct file));
		return (kfree(node_ab), kfree(socket), kfree(node_ba), -1);
	}
	kernel_socket[1] = put_file(file_b, process);
	if (kernel_socket[1] == -1)
	{
		kmemset(file_a, 0, sizeof(struct file));
		kmemset(file_b, 0, sizeof(struct file));
		fd_free(process, kernel_socket[0]);
		return (kfree(node_ab), kfree(socket), kfree(node_ba), -1);
	}

	if (copy_to_user_wrap(sv, kernel_socket, sizeof(kernel_socket)) == -1)
	{
		kmemset(file_a, 0, sizeof(struct file));
		kmemset(file_b, 0, sizeof(struct file));
		fd_free(process, kernel_socket[0]);
		fd_free(process, kernel_socket[1]);
		return (kfree(node_ab), kfree(socket), kfree(node_ba), -1);
	}
	return (0);
}

ssize_t sendmsg(int fd_socket, const struct msghdr *msg, int flags)
{
	proc_t *process = get_current_process();
	ssize_t	bytes_read = 0;
	struct file *file = fd_get(process, fd_socket);
	if (file == NULL)
		return (-1);

	struct socket_channel *socket = (struct socket_channel *)file->node->fs_info;
	struct socket_direct *direction;
	if (file->flags == O_SOCKET_SIDE_A)
		direction = &socket->socket_ab;
	else
		direction = &socket->socket_ba;

	if (flags == SCM_RIGHTS)
	{
		int i;
		for (i = 0; i < 8; i++)
		{
			if (direction->shared_fds[i] == NULL)
			{
				int fd = (int)msg->msg_control;
				direction->shared_fds[i] = fd_get(process, fd);
				if (direction->shared_fds[i] == NULL)
					return (-1);
				break;
			}
		}
		if (i == 8)
			return (-1);
	}
	struct buf_ring *buffer = &direction->text_buff;
	for (size_t i = 0; i < msg->msg_iovlen; i++)
	{
		struct iovec *vec = msg->msg_iov + i;
		for (size_t j = 0; j < vec->iov_len; j++)
		{
			if (buffer->size > buffer->count)
			{
				if (copy_from_user_wrap(&buffer->buff[buffer->head], &((char*)vec->iov_base)[j], 1) == -1)
					return (-1);
				buffer->head = (buffer->head + 1) % buffer->size;
				buffer->count++;
				bytes_read++;
			}
		}
	}
	return (bytes_read);
}

ssize_t	recvmsg(int fd_socket, struct msghdr *msg, int flags)
{
	proc_t *process = get_current_process();
	int bytes_write = 0;
	struct file *file = fd_get(process, fd_socket);
	if (file == NULL)
		return (-1);
	struct socket_channel *socket = (struct socket_channel*)file->node->fs_info;
	struct socket_direct *direction;
	if (file->flags == O_SOCKET_SIDE_A)
		direction = &socket->socket_ba;
	else
		direction = &socket->socket_ab;

	if (flags == SCM_RIGHTS)
	{
		int i;
		for (i = 0; i < 8; i++)
		{
			if (direction->shared_fds[i] != NULL)
			{
				int32_t fd = put_file(direction->shared_fds[i], process);
				if (fd == -1)
					return (-1);
				direction->shared_fds[i] = NULL;
				msg->msg_control = (void*)fd;
				break;
			}
		}
		if (i == 8)
			return (-1);
	}
	struct buf_ring *buffer = &direction->text_buff;
	for (size_t i = 0; i < msg->msg_iovlen; i++)
	{
		struct iovec *vec = msg->msg_iov + i;
		for (size_t j = 0; j < vec->iov_len; j++)
		{
			if (buffer->count > 0)
			{
				if (copy_to_user_wrap(&((char*)vec->iov_base)[j], &buffer->buff[buffer->tail], 1) == -1)
					return (-1);
				buffer->tail = (buffer->tail + 1) % buffer->size;
				buffer->count--;
				bytes_write++;
			}
		}
	}
	return (bytes_write);
}

