#include "drivers/tty/tty.h"

static struct tty *tty;

void	init_tty()
{
	kmemset(tty->buff, 0, 256);
	tty->head = 0;
	tty->tail = 0;
}

void	tty_push_char(char c)
{
	asm volatile("cli");

	if (tty->head == tty->tail)
		return ;
	if (((tty->head + 1) % 256) == tty->tail)
		kmemset(tty->buff, 0, 256);
	tty->buff[tty->head] = c;
	tty->head = (tty->head + 1) % 256;

	asm volatile("sti");
}

char	tty_read_char()
{
	asm volatile("cli");

	if (tty->head == tty->tail)
		return (' ');
	char c = tty->buff[tty->tail];
	tty->tail = (tty->tail + 1) % 256;

	asm volatile("sti");
	return (c);
}

