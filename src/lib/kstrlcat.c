#include <utils.h>

char	*kstrlcat(char *dest, const char *str, size_t size)
{
	size_t n = 0;
	size_t i = 0;

	if (size == 0)
		return (NULL);
	while (dest[i] != '\0')
		i++;
	
	while (str[n] != '\0' && i + 1 < size)
	{
		dest[i] = str[n];
		i++;
		n++;
	}
	dest[i] = '\0';
	return (dest);
}

