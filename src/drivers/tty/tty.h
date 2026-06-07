#ifndef TTY_H
#define TTY_H

#include <utils.h>
#include "fs/vfs/vfs.h"

struct vfs_node;

struct tty
{
	char 		buff[256];
	uint32_t	head;
	uint32_t	tail;
};

void	tty_push_char(char c);
char 	tty_read_char();

ssize_t	tty_vfs_write(struct vfs_node *node, char *str, size_t len, size_t offset);
ssize_t	tty_vfs_read(struct vfs_node *node, char *buff, size_t len, size_t offset);

#endif
