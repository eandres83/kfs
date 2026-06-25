#include <utils.h>

size_t	kstrlen(const char *str)
{
	size_t	len = 0;

	while (str[len])
		len++;
	return (len);
}

size_t _api_kstrlen(const char *str) __attribute__((alias("kstrlen")));

