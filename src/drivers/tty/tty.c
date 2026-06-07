#include "drivers/tty/tty.h"
#include "drivers/vga.h"

static struct tty tty_inst;

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

ssize_t	tty_vfs_write(struct vfs_node *node, char *str, size_t len, size_t offset)
{
	(void)node;
	(void)offset;
	terminal_write(str, len);
	return (len);
}

ssize_t	tty_vfs_read(struct vfs_node *node, char *buff, size_t len, size_t offset)
{
	(void)node;
	(void)buff;
	(void)offset;
	(void)len;
	buff[0] = tty_read_char();
	return (1);
}

