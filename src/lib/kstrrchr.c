#include <utils.h>

size_t	kstrrchr(const char *str, char c)
{
	size_t	len;

	len = kstrlen(str) + 1;
	while (len--)
	{
		if (*(str + len) == (char)c)
			return (len);
	}
	return (-1);
}

size_t _api_kstrrchr(const char *str, char c) __attribute__((alias("kstrrchr")));

