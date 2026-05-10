#include "arch/i386/lib/uaccess.h"

ssize_t read(int fd, void *buf, size_t count)
{
	if (fd != 0)
		return (-1);

	char kbuff[256] = {0};
	for (size_t i = 0; i < count; i++)
		kbuff[i] = tty_read_char();

	copy_to_user(kbuff, (char*)buf, count);
	return (count);
}

ssize_t	write(int fd, const char *str, size_t count)
{
	if (fd != 1)
		return (-1);
	terminal_write(str, count);
	return (count);
}

