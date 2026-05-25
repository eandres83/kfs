#ifndef SYSFILE_H
#define SYSFILE_H

#include <utils.h>
#include "task/task.h"

ssize_t dup(int32_t fd);
ssize_t dup2(int32_t oldfd, int32_t newfd);
ssize_t	pipe(int *fd);

#endif
