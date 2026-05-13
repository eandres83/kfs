#include "fs/fd.h"
#include "task/task.h"

int32_t	fd_allocate(proc_t *proc)
{
	struct file *new_fd = kmalloc(sizeof(struct file));
	if (!new_fd)
		return (-1);

	kmemset(new_fd, 0, sizeof(struct file));

	for (int i = 0; i < MAX_FD; i++)
	{
		if (proc->fd_table[i] == NULL)
		{
			proc->fd_table[i] = new_fd;
			return (i);
		}
	}
	return (-1);
}

struct file	*fd_get(proc_t *proc, int fd)
{
	if (fd > 63 || fd < 0)
		return (NULL);
	struct file *new_file = proc->fd_table[fd];
	return (new_file);
}

void	fd_free(proc_t *proc, int fd)
{
	if (fd > 63 || fd < 0)
		return ;
	kfree(proc->fd_table[fd]);
	proc->fd_table[fd] = NULL;
}

