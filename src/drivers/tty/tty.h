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

void	init_tyy();
void	tty_push_char(char c);
char 	tty_read_char();

size_t	tty_vfs_write(struct vfs_node *node, char *str, size_t len);
char	*tty_vfs_read(struct vfs_node *node);

#endif
