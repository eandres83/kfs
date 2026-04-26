#include <utils.h>

size_t	kstrrchr(const char *str, int c)
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

