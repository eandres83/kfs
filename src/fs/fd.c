#include "fs/fd.h"
#include "task/task.h"

#define MAX_OPEN_FILES 64

struct file global_files[MAX_OPEN_FILES];

struct file *file_allocate()
{
	for (int i = 0; i < MAX_OPEN_FILES; i++)
	{
		if (global_files[i].ref_count == 0)
		{
			kmemset(&global_files[i], 0, sizeof(struct file));
			global_files[i].ref_count = 1;
			return (&global_files[i]);
		}
	}
	return (NULL);
}

int32_t	fd_allocate(proc_t *proc)
{
	struct file *new_fd = file_allocate();
	if (!new_fd)
		return (-1);

	for (int i = 0; i < MAX_FD; i++)
	{
		if (proc->fd_table[i] == NULL)
		{
			proc->fd_table[i] = new_fd;
			return (i);
		}
	}
	new_fd->ref_count = 0;
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
	if (fd >= MAX_FD || fd < 0 || proc->fd_table[fd] == NULL)
		return ;
	struct file *global_file = proc->fd_table[fd];
	global_file->ref_count--;
	proc->fd_table[fd] = NULL;

	if (global_file->ref_count == 0)
	{
		if (global_file->node)
		{
			if (global_file->node->ops->close != NULL)
				global_file->node->ops->close(global_file->node);
		}
		kmemset(global_file, 0, sizeof(struct file));
	}
}

static struct ops tty_fake_ops = {
	.read = tty_vfs_read,
	.write = tty_vfs_write,
	.open = NULL,
	.close = tty_vfs_close,
	.readdir = NULL,
	.finddir = NULL,
	.stat = NULL,
};

static struct vfs_node *alloc_node()
{
	struct vfs_node *node = kmalloc(sizeof(struct vfs_node));
	if (!node)
		return (NULL);
	kmemset(node, 0, sizeof(struct vfs_node));
	node->ops = &tty_fake_ops;
	return (node);
}

void	create_init_fd(proc_t *proc)
{
	struct file *file_stdin = file_allocate();
	if (!file_stdin)
		return ;
	file_stdin->flags = O_RDONLY;
	file_stdin->node = alloc_node();
	proc->fd_table[0] = file_stdin;

	struct file *file_stdout = file_allocate();
	if (!file_stdout)
		return ;
	file_stdout->flags = O_WRONLY;
	file_stdout->node = alloc_node();
	proc->fd_table[1] = file_stdout;

	struct file *file_stderr = file_allocate();
	if (!file_stderr)
		return ;
	file_stderr->node = alloc_node();
	file_stderr->flags = O_WRONLY;
	proc->fd_table[2] = file_stderr;
}

