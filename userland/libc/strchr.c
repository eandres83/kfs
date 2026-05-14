#include "../minilib.h"

char	*strchr(const char *str, char c)
{
	size_t len;

	len = 0;
	while (str[len] != '\0')
	{
		if (str[len] == c)
			return ((char*)&str[len]);
		len++;
	}
	return (0);
}

