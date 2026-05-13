#include "../minilib.h"

size_t	strrchr(const char *str, char c)
{
	size_t	len;

	len = strlen(str) + 1;
	while (len--)
	{
		if (*(str + len) == (char)c)
			return (len);
	}
	return (-1);
}

