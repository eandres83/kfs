#include "drivers/tty/tty.h"

static struct tty tty_inst;

void	init_tty()
{
	kmemset(tty_inst.buff, 0, 256);
	tty_inst.head = 0;
	tty_inst.tail = 0;
}

void	tty_push_char(char c)
{
	asm volatile("cli");

	if (((tty_inst.head + 1) % 256) == tty_inst.tail)
		return ;
	if (tty_inst.head == tty_inst.tail)
		kmemset(tty_inst.buff, 0, 256);
	tty_inst.buff[tty_inst.head] = c;
	tty_inst.head = (tty_inst.head + 1) % 256;

	asm volatile("sti");
}

char	tty_read_char()
{
	asm volatile("cli");

	while (tty_inst.head == tty_inst.tail)
	{
		asm volatile("sti");
		asm volatile("hlt");
		asm volatile("cli");
	}

	char c = tty_inst.buff[tty_inst.tail];
	tty_inst.tail = (tty_inst.tail + 1) % 256;

	asm volatile("sti");
	return (c);
}

size_t	tty_vfs_write(struct vfs_node *node, char *str, size_t len)
{
	(void)node;
	terminal_write(str, len);
	return (1);
}

char	*tty_vfs_read(struct vfs_node *node)
{
	(void)node;
	char *buff = (char*)kmalloc(3);
	if (!buff)
		return (NULL);

	buff[0] = tty_read_char();
	terminal_putchar(buff[0]);
	buff[1] = '\0';
	return (buff);
}

