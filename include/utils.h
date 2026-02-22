#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

#define PANIC(msg) panic(msg, __FILE__, __LINE__)

// noreturn es para que el compilador sepa que el programa acaba siempre que se llama a esta funcion
void	panic(char *msg, const char *file, uint32_t line) __attribute__((noreturn));

size_t	kstrlen(const char *str);
void	*kmemcpy(void *dest, const void *src, size_t n);
void	*kmemset(void *dest, int c, size_t n);
void	kputnbr(long long n, int base);
void	kprintf(const char *format, ...);
int	kstrcmp(const char *s1, const char *s2);
char	*kstrcpy(char *dest, const char src);

#endif
