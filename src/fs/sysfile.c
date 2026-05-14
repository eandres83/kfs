#include "sysfile.h"

// this is only for test that minishell compile

ssize_t dup(uint32_t fd)
{
	(void)fd;
	kprintf("Esta mierda es el dup :)\n");
	return (0);
}

ssize_t dup2(uint32_t oldfd, uint32_t newfd)
{
	(void)oldfd;
	(void)newfd;
	kprintf("Esto es otra mierda mejor dup2 ;)\n");
	return (0);
}

ssize_t pipe(int *fd)
{
	(void)fd;
	kprintf("Esto es la magia del pipe\n");
	return (0);
}

