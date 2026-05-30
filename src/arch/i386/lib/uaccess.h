#ifndef UACCESS
#define UACCESS

#include <utils.h>
#include <uapi.h>
#include "drivers/vga.h"
#include "drivers/tty/tty.h"
#include "mm/vmm.h"
#include "task/task.h"
#include "fs/fd.h"

ssize_t	copy_to_user_wrap(void *user_addr, const void *kernel_addr, size_t count);
ssize_t	copy_from_user_wrap(void *kernel_addr, const void *user_addr, size_t count);

// syscall
ssize_t	read(int fd, void *buffer, size_t count);
ssize_t	write(int fd, const char *buf, size_t count);
ssize_t	open(char *filename, uint32_t flags, uint32_t mode);
ssize_t	close(int fd);
ssize_t access(char *filename, int mode);

#endif
