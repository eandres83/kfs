#include "sysfile.h"

// this is only for test that minishell compile

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

static struct buf_ring pipe_buf;

void	init_buf_ring(int size)
{
	kmemset(pipe_buf.buff, 0, size);
	pipe_buf.head = 0;
	pipe_buf.tail = 0;
	pipe_buf.size = size;
	pipe_buf.count = 0;
}

char *pipe_read(struct vfs_node *node)
{
	(void)node;
	if (pipe_buf.count > 0)
	{
		pipe_buf.tail = (pipe_buf.tail + 1) % pipe_buf.size;
		pipe_buf.count--;
	}
	return (NULL);
}

size_t pipe_write(struct vfs_node *node, char *str, size_t size)
{
	(void)node;
	for (size_t i = 0; str[i] && size > i; i++)
	{
		if (pipe_buf.size > pipe_buf.count)
		{
//			copy_to_user(str[i], pipe_buf.buff[pipe_buf.head], 1);
			pipe_buf.count++;
			pipe_buf.head = (pipe_buf.head + 1) % pipe_buf.size;
		}
	}
	return (-1);
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
	proc_t *proc = get_current_process();
	if (proc->fd_table[fd[0]] == NULL || proc->fd_table[fd[1]] == NULL)
		return (-1);
	struct vfs_node *node = kmalloc(sizeof(struct vfs_node));
	if (!node)
		return (-1);
	kmemset(node, 0, sizeof(struct vfs_node));
	node->ops = &buf_ring;

	struct file *file_read = kmalloc(sizeof(struct file));
	if (!file_read)
		return (-1);
	kmemset(file_read, 0, sizeof(struct file));
	file_read->ref_count++;
	file_read->flags = O_RDONLY;
	file_read->node = node;

	struct file *file_write = kmalloc(sizeof(struct file));
	if (!file_write)
		return (-1);
	kmemset(file_write, 0, sizeof(struct file));
	file_write->ref_count++;
	file_write->flags = O_WRONLY;
	file_write->node = node;

//	proc->fd_table[fd[0]] = file_read;
//	proc->fd_table[fd[1]] = file_write;
//	fd[0] = 
	return (0);
}

