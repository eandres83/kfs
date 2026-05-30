#include "sysfile.h"

ssize_t dup(int32_t fd)
{
	proc_t *proc = get_current_process();
	if ((fd >= MAX_FD || fd < 0) || proc->fd_table[fd] == NULL)
		return (-1);

	for (int32_t i = 0; i < MAX_FD; i++)
	{
		if (proc->fd_table[i] == NULL)
		{
			proc->fd_table[i] = proc->fd_table[fd];
			proc->fd_table[i]->ref_count++;
			return (i);
		}
	}
	return (-1);
}

ssize_t dup2(int32_t oldfd, int32_t newfd)
{
	proc_t *proc = get_current_process();
	if ((oldfd >= MAX_FD || oldfd < 0) || proc->fd_table[oldfd] == NULL)
		return (-1);
	if (newfd >= MAX_FD || newfd < 0)
		return (-1);

	if (oldfd == newfd)
		return (newfd);
	if (proc->fd_table[newfd] != NULL)
		fd_free(proc, newfd);
	proc->fd_table[newfd] = proc->fd_table[oldfd];
	proc->fd_table[newfd]->ref_count++;
	return (newfd);
}

ssize_t pipe_read(struct vfs_node *node, char *buff, size_t len, size_t offset)
{
	(void)node;
	(void)offset;
	struct buf_ring *pipe_buf = (struct buf_ring *)node->fs_info;
	size_t i;
	for (i = 0; len > i; i++)
	{
		if (pipe_buf->count > 0)
		{
			kmemcpy(&buff[i], &pipe_buf->buff[pipe_buf->tail], 1);
			pipe_buf->tail = (pipe_buf->tail + 1) % pipe_buf->size;
			pipe_buf->count--;
		}
	}
	return (i);
}

ssize_t pipe_write(struct vfs_node *node, char *str, size_t size, size_t offset)
{
	(void)node;
	(void)offset;
	struct buf_ring *pipe_buf = (struct buf_ring *)node->fs_info;
	size_t i;
	for (i = 0; size > i; i++)
	{
		if (pipe_buf->size > pipe_buf->count)
		{
			kmemcpy(&pipe_buf->buff[pipe_buf->head], &str[i], 1);
			pipe_buf->head = (pipe_buf->head + 1) % pipe_buf->size;
			pipe_buf->count++;
		}
	}
	return (i);
}

static struct ops buf_ring = {
	.read = pipe_read,
	.write = pipe_write,
	.open = NULL,
	.close = NULL,
	.readdir = NULL,
	.finddir = NULL,
};

ssize_t pipe(int *fd)
{
	int kernel_fd[2];
	proc_t *proc = get_current_process();
	struct vfs_node *node = kmalloc(sizeof(struct vfs_node));
	if (!node)
		return (-1);
	kmemset(node, 0, sizeof(struct vfs_node));
	node->ops = &buf_ring;

	struct buf_ring *buf_ring = kmalloc(sizeof(struct buf_ring));
	if (!buf_ring)
		return (kfree(node), -1);
	kmemset(buf_ring, 0, sizeof(struct buf_ring));
	buf_ring->size = 4096;
	node->fs_info = buf_ring;

	kernel_fd[0] = fd_allocate(proc);
	if (kernel_fd[0] == -1)
		return (kfree(buf_ring), kfree(node), -1);
	struct file *file_read = proc->fd_table[kernel_fd[0]];
	file_read->flags = O_RDONLY;
	file_read->node = node;

	kernel_fd[1] = fd_allocate(proc);
	if (kernel_fd[1] == -1)
		return (kfree(buf_ring), kfree(node), fd_free(proc, kernel_fd[0]), -1);
	struct file *file_write = proc->fd_table[kernel_fd[1]];
	file_write->flags = O_WRONLY;
	file_write->node = node;

	if (copy_to_user_wrap(fd, kernel_fd, sizeof(kernel_fd)) == -1)
	{
		fd_free(proc, kernel_fd[0]);
		fd_free(proc, kernel_fd[1]);
		return (kfree(buf_ring), kfree(node), -1);
	}
	kprintf("el valor de los fd asignados -> %d -> %d\n", fd[0], fd[1]);
	return (0);
}

