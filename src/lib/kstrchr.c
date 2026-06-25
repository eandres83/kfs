#include <utils.h>

size_t	kstrchr(const char *str, char c)
{
	size_t len;

	len = 0;
	while (str[len] != '\0')
	{
		if (str[len] == c)
			return (len);
		len++;
	}
	return (-1);
}

size_t _api_kstrchr(const char *str, char c) __attribute__((alias("kstrchr")));

