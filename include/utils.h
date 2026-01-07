#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

size_t	kstrlen(const char *str);
void	*kmemcpy(void *dest, const void *src, size_t n);
void	*kmemset(void *dest, int c, size_t n);
void	kputnbr(long long n, int base);
void	kprintf(const char *format, ...);

#endif
