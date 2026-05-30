#ifndef SYSFILE_H
#define SYSFILE_H

#include <utils.h>
#include "task/task.h"
#include "arch/i386/lib/uaccess.h"

struct buf_ring
{
	char		buff[4096];
	uint32_t	head;
	uint32_t	tail;
	uint32_t	size;
	uint32_t	count;
};

ssize_t dup(int32_t fd);
ssize_t dup2(int32_t oldfd, int32_t newfd);
ssize_t	pipe(int *fd);

// buffer ring function
void	init_buf_ring(int size);

#endif
