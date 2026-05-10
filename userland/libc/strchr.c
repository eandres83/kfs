#include "../minilib.h"

size_t	strchr(const char *str, char c)
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

