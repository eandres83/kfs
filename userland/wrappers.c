#include "minilib.h"

void exit(uint32_t status)
{
	asm volatile("int $0x80" : : "a" (SYS_exit), "b" (status) : "memory");
	while (1);
}

ssize_t fork()
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_fork) : "memory");
	return (ret);
}

ssize_t read(size_t fd, char *buf, size_t len)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_read), "b" (fd), "c" (buf), "d" (len) : "memory");
	return (ret);
}

ssize_t write(size_t fd, const char *str, size_t len)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_write), "b" (fd), "c" (str), "d" (len) : "memory");
	return (ret);
}

ssize_t open(char *filename, uint32_t flags, uint32_t mode)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_open), "b" (filename), "c" (flags), "d" (mode) : "memory");
	return (ret);
}

ssize_t close(uint32_t fd)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_close), "b" (fd) : "memory");
	return (ret);
}

ssize_t waitpid(ssize_t pid, int *status, uint32_t options)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_waitpid), "b" (pid), "c" (status), "d" (options) : "memory");
	return (ret);
}

ssize_t wait(int *status)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_wait), "b" (status) : "memory");
	return (ret);
}

ssize_t execve(char *file_path, char **argv, char **envp)
{
	ssize_t	ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_execve), "b" (file_path), "c" (argv), "d" (envp) : "memory");
	return (ret);
}

void getuid()
{
	asm volatile("int $0x80" : : "a" (SYS_getuid) : "memory");
}

ssize_t access(char *file_name, int mode)
{
	ssize_t	ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_access), "b" (file_name), "c" (mode) : "memory");
	return (ret);
}

ssize_t kill(uint32_t pid, uint32_t sig)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_kill), "b" (pid), "c" (sig) : "memory");
	return (ret);
}

ssize_t dup(uint32_t fd)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_dup), "b" (fd) : "memory");
	return (ret);
}

ssize_t	pipe(int *fd)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_pipe), "b" (fd) : "memory");
	return (ret);
}

ssize_t signal(uint32_t num, void (*function)())
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_signal), "b" (num), "c" (function) : "memory");
	return (ret);
}

ssize_t dup2(uint32_t oldfd, uint32_t newfd)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_dup2), "b" (oldfd), "c" (newfd) : "memory");
	return (ret);
}

void	*mmap(size_t size)
{
	void *ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_mmap), "b" (size) : "memory");
	return (ret);
}

ssize_t	munmap(void *addr, size_t size)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_munmap), "b" (addr), "c" (size) : "memory");
	return (ret);
}

char	*getcwd(void *buff, size_t size)
{
	char *ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (SYS_getcwd), "b" (buff), "c" (size) : "memory");
	if (ret == 0)
		return (NULL);
	return (ret);
}

