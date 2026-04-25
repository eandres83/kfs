#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <kmalloc.h>

typedef int32_t ssize_t;

#define PANIC(msg) panic(msg, __FILE__, __LINE__)

// noreturn es para que el compilador sepa que el programa acaba siempre que se llama a esta funcion
void	panic(char *msg, const char *file, uint32_t line) __attribute__((noreturn));

size_t	kstrlen(const char *str);
void	*kmemcpy(void *dest, const void *src, size_t n);
void	*kmemset(void *dest, int c, size_t n);
void	kputnbr(long long n, int base);
void	kprintf(const char *format, ...);
int	kstrcmp(const char *s1, const char *s2);
char	*kstrcpy(char *dest, const char *src);
size_t	kstrlcpy(char *dest, const char *src, size_t size);
char 	**ksplit(const char *str, char c);
void	double_free(char **str);
size_t	kstrncmp(const char *s1, const char *s2, size_t n);
char 	*kstrlcat(char *dest, const char *str, size_t size);

#endif
