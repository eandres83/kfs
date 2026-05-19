#ifndef MINILIB_H
#define MINILIB_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include "malloc/malloc.h"
#include "../include/uapi.h"

// wrappers for syscall
void	exit(uint32_t status);
ssize_t	fork();
ssize_t	read(size_t fd, char *buf, size_t len);
ssize_t	write(size_t fd, const char *str, size_t len);
ssize_t open(char *filename, uint32_t flags, uint32_t mode);
ssize_t close(uint32_t fd);
ssize_t	wait(int *status);
ssize_t	kill(uint32_t pid, uint32_t sig);
ssize_t	execve(char *file_path, char **argv, char **envp);
ssize_t	access(char *file_name, int mode);
ssize_t	dup(uint32_t fd);
ssize_t	pipe(int *fd);
ssize_t	signal(uint32_t num, void (*function)());
ssize_t	dup2(uint32_t oldfd, uint32_t newfd);
void	*mmap(size_t size);
ssize_t munmap(void *addr, size_t size);
void	getuid();
char	*getcwd(void *buff, size_t size);

// other function
void	printf(const char *format, ...);

// libc
size_t	atoi(char *str);
void	*memcpy(void *dest, const void *src, size_t n);
void	*memset(void *dest, int c, size_t n);
void	putnbr(long n, int base);
char	**split(const char *s, char c);
char	*strchr(const char *str, char c);
int	strcmp(const char *s1, const char *s2);
char	*strcpy(char *dest, const char *src);
char	*strcat(char *dest, const char *str);
size_t	strlcpy(char *dest, const char *src, size_t size);
size_t	strlen(const char *str);
size_t	strncmp(const char *s1, const char *s2, size_t n);
size_t	strrchr(const char *str, char c);
char	*strdup(const char *src);
char	*strjoin(const char *s1, const char *s2);
char	*substr(const char *s, unsigned int start, size_t len);
size_t	strlcat(char *dest, const char *src, size_t size);
char	*itoa(int num);
void	*bzero(void *s, size_t n);
int	isalnum(char c);
int	isalpha(char c);
int 	isdigit(char c);

#endif
