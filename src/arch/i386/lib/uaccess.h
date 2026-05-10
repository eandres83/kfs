#ifndef UACCESS
#define UACCESS

#include <utils.h>
#include "drivers/vga.h"
#include "drivers/tty/tty.h"
#include "mm/vmm.h"

ssize_t	copy_to_user(char *kernel, char *str, size_t count);
ssize_t	read(int fd, void *buffer, size_t count);
ssize_t	write(int fd, const char *buf, size_t count);

#endif
