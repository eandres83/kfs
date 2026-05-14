#include "../minilib.h"

size_t	strlcat(char *dest, const char *src, size_t size)
{
	size_t	n = 0;
	size_t	i = 0;
	size_t	ret;

	if (size == 0)
		return (strlen(src));
	else if (size < strlen(dest))
		ret= strlen(src) + size;
	else
		ret = strlen(dest) + strlen(src);
	while (dest[i] != '\0')
		i++;
	while (src[n] != '\0' && i + 1 < size)
	{
		dest[i] = src[n];
		i++;
		n++;
	}
	dest[i] = '\0';
	return (ret);
}

