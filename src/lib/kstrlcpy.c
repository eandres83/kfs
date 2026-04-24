#include <utils.h>

size_t kstrlcpy(char *dest, const char *src, size_t size)
{
	size_t n = 0;
	size_t i = 0;

	while (src[n] != '\0')
		n++;
	while (src[i] != '\0' && i + 1 < size)
	{
		dest[i] = src[i];
		i++;
	}
	if (size)
		dest[i] = '\0';
	return (n);
}

