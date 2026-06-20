#ifndef MODULE_H
#define MODULE_H

#include <utils.h>

size_t	_api_kstrlen(const char *str);
#define kstrlen _api_kstrlen

void	*_api_kmemcpy(void *dest, const void *src, size_t n);
#define kmemcpy _api_kmemcpy

void	*_api_kmemset(void *dest, int c, size_t n);
#define kmemset _api_kmemset

void	_api_kputnbr(long long n, int base);
#define kputnbr _api_kputnbr

void	_api_kprintf(const char *format, ...);
#define kprintf _api_kprintf

int	_api_kstrcmp(const char *s1, const char *s2);
#define kstrcmp _api_kstrcmp

char	*_api_kstrcpy(char *dest, const char *src);
#define kstrcpy _api_kstrcpy

size_t	_api_kstrlcpy(char *dest, const char *src, size_t size);
#define kstrlcpy _api_kstrlcpy

char 	**_api_ksplit(const char *str, char c);
#define ksplit _api_ksplit

size_t	_api_kstrncmp(const char *s1, const char *s2, size_t n);
#define kstrncmp _api_kstrncmp

char 	*_api_kstrcat(char *dest, const char *str);
#define kstrcat _api_kstrcat

size_t	_api_kstrrchr(const char *str, char c);
#define kstrrchr _api_kstrrchr

size_t	_api_kstrchr(const char *str, char c);
#define kstrchr _api_kstrchr

size_t	_api_katoi(char *str);
#define katoi _api_katoi

#endif
