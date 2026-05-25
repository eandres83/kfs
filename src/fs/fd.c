#include "fs/fd.h"
#include "task/task.h"

int32_t	fd_allocate(proc_t *proc)
{
	struct file *new_fd = kmalloc(sizeof(struct file));
	if (!new_fd)
		return (-1);

	kmemset(new_fd, 0, sizeof(struct file));
	new_fd->ref_count++;

	for (int i = 0; i < MAX_FD; i++)
	{
		if (proc->fd_table[i] == NULL)
		{
			proc->fd_table[i] = new_fd;
			return (i);
		}
	}
	kfree(new_fd);
	return (-1);
}

struct file	*fd_get(proc_t *proc, int fd)
{
	if (fd >= MAX_FD || fd < 0)
		return (NULL);
	struct file *new_file = proc->fd_table[fd];
	return (new_file);
}

void	fd_free(proc_t *proc, int fd)
{
	if (fd >= MAX_FD || fd < 0)
		return ;
	if (proc->fd_table[fd] == NULL)
		return ;
	proc->fd_table[fd]->ref_count--;
	if (proc->fd_table[fd]->ref_count == 0)
		kfree(proc->fd_table[fd]);
	proc->fd_table[fd] = NULL;
}

static struct ops tty_fake_ops = {
	.read = tty_vfs_read,
	.write = tty_vfs_write,
	.open = NULL,
	.close = NULL,
	.readdir = NULL,
	.finddir = NULL,
};

void	create_init_fd(proc_t *proc)
{
	struct vfs_node *node = kmalloc(sizeof(struct vfs_node));
	if (!node)
		return ;
	kmemset(node, 0, sizeof(struct vfs_node));
	node->ops = &tty_fake_ops;

	struct file *file_stdin = kmalloc(sizeof(struct file));
	if (!file_stdin)
		return ;
	kmemset(file_stdin, 0, sizeof(struct file));
	file_stdin->ref_count++;
	file_stdin->flags = O_RDONLY;
	file_stdin->node = node;

	struct file *file_stdout = kmalloc(sizeof(struct file));
	if (!file_stdout)
		return ;
	kmemset(file_stdout, 0, sizeof(struct file));
	file_stdout->ref_count++;
	file_stdout->flags = O_WRONLY;
	file_stdout->node = node;

	struct file *file_stderr = kmalloc(sizeof(struct file));
	if (!file_stderr)
		return ;
	kmemset(file_stderr, 0, sizeof(struct file));
	file_stderr->ref_count++;
	file_stderr->node = node;
	file_stderr->flags = O_WRONLY;

	proc->fd_table[0] = file_stdin;
	proc->fd_table[1] = file_stdout;
	proc->fd_table[2] = file_stderr;
}

