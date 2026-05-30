#ifndef FD_H
#define FD_H

#include <utils.h>
#include <uapi.h>
#include "drivers/tty/tty.h"

struct proc;

struct file
{
	struct vfs_node *node;
	uint32_t	offset;
	uint32_t	flags;
	uint32_t	ref_count;
};

int32_t		fd_allocate(struct proc *proc);
struct file	*fd_get(struct proc *proc, int fd);
void		fd_free(struct proc *proc, int fd);
void		create_init_fd(struct proc *proc);

#endif
