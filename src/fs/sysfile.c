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

ssize_t pipe(int *fd)
{
	(void)fd;
	kprintf("Esto es la magia del pipe\n");
	return (0);
}

