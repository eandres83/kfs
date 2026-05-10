#ifndef MINILIB_H
#define MINILIB_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "malloc/malloc.h"

// wrappers for syscall
ssize_t	write(size_t fd, char *str, size_t len);
void	exit(uint32_t status);
ssize_t	fork();
ssize_t	wait(uint32_t *status);
ssize_t	kill(uint32_t pid, uint32_t sig);
ssize_t	execve(char *file_path, char **argv, char **envp);
ssize_t	signal(uint32_t num, void (*function)());
ssize_t	mmap(size_t size);
ssize_t munmap(void *addr, size_t size);
void	getuid();

// other function
void	printf(const char *format, ...);

// libc
size_t	atoi(char *str);
void	*memcpy(void *dest, const void *src, size_t n);
void	*memset(void *dest, int c, size_t n);
void	putnbr(long long n, int base);
char	**split(const char *s, char c);
size_t	strchr(const char *str, char c);
int	strcmp(const char *s1, const char *s2);
char	*strcpy(char *dest, const char *src);
char	*strcat(char *dest, const char *str);
size_t	strlcpy(char *dest, const char *src, size_t size);
size_t	strlen(const char *str);
size_t	strncmp(const char *s1, const char *s2, size_t n);
size_t	strrchr(const char *str, char c);

#endif
