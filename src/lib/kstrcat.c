#include <utils.h>

char	*kstrcat(char *dest, const char *str)
{
	size_t n = 0;
	size_t i = 0;

	while (dest[i] != '\0')
		i++;
	while (str[n] != '\0')
	{
		dest[i] = str[n];
		i++;
		n++;
	}
	dest[i] = '\0';
	return (dest);
}

