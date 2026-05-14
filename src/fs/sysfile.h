#ifndef SYSFILE_H
#define SYSFILE_H

#include <utils.h>

ssize_t dup(uint32_t fd);
ssize_t dup2(uint32_t oldfd, uint32_t newfd);
ssize_t	pipe(int *fd);

#endif
