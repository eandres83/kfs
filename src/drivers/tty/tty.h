#ifndef TTY_H
#define TTY_H

#include <utils.h>

struct tty
{
	char 		buff[256];
	uint32_t	head;
	uint32_t	tail;
};

void	init_tyy();
void	tty_push_char(char c);
char 	tty_read_char();

#endif
