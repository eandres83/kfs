#ifndef SYSFILE_H
#define SYSFILE_H

#include <utils.h>
#include <uapi.h>

struct buf_ring
{
	char		buff[4096];
	uint32_t	head;
	uint32_t	tail;
	uint32_t	size;
	uint32_t	count;
	uint32_t	fds;
};

ssize_t dup(int32_t fd);
ssize_t dup2(int32_t oldfd, int32_t newfd);
ssize_t	pipe(int *fd);

// buffer ring function
void	init_buf_ring(int size);

ssize_t	stat_func(const char *path, struct stat *statbuf);
ssize_t	fstat_func(int fd, struct stat *user_stat);
ssize_t	lstat_func(const char *path, struct stat *user_stat);

ssize_t	getdents(int fd, struct linux_dirent *dirp, int count);

#endif
