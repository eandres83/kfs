#include "minilib.h"

void exit(uint32_t status)
{
	asm volatile("int $0x80" : : "a" (1), "b" (status) : "memory");
	while (1);
}

ssize_t fork()
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (2) : "memory");
	return (ret);
}

ssize_t read(size_t fd, char *buf, size_t len)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (3), "b" (fd), "c" (buf), "d" (len) : "memory");
	return (ret);
}

ssize_t write(size_t fd, const char *str, size_t len)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (4), "b" (fd), "c" (str), "d" (len) : "memory");
	return (ret);
}

ssize_t open(char *filename, uint32_t flags, uint32_t mode)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (5), "b" (filename), "c" (flags), "d" (mode) : "memory");
	return (ret);
}

ssize_t close(uint32_t fd)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (6), "b" (fd) : "memory");
	return (ret);
}

ssize_t wait(int *status)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (7), "b" (status) : "memory");
	return (ret);
}

ssize_t kill(uint32_t pid, uint32_t sig)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (37), "b" (pid), "c" (sig) : "memory");
	return (ret);
}

ssize_t execve(char *file_path, char **argv, char **envp)
{
	ssize_t	ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (11), "b" (file_path), "c" (argv), "d" (envp) : "memory");
	return (ret);
}

ssize_t access(char *file_name, int mode)
{
	ssize_t	ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (33), "b" (file_name), "c" (mode) : "memory");
	return (ret);
}

void getuid()
{
	asm volatile("int $0x80" : : "a" (24) : "memory");
}

ssize_t dup(uint32_t fd)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (41), "b" (fd) : "memory");
	return (ret);
}

ssize_t	pipe(int *fd)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (42), "b" (fd) : "memory");
	return (ret);
}

ssize_t signal(uint32_t num, void (*function)())
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (48), "b" (num), "c" (function) : "memory");
	return (ret);
}

ssize_t dup2(uint32_t oldfd, uint32_t newfd)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (63), "b" (oldfd), "c" (newfd) : "memory");
	return (ret);
}

void	*mmap(size_t size)
{
	void *ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (90), "b" (size) : "memory");
	return (ret);
}

ssize_t	munmap(void *addr, size_t size)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (91), "b" (addr), "c" (size) : "memory");
	return (ret);
}

char	*getcwd(void *buff, size_t size)
{
	char *ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (183), "b" (buff), "c" (size) : "memory");
	return (ret);
}

