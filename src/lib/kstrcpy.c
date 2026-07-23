#include <sys/kmalloc.h>

char *kstrcpy(char *dest, const char *src)
{
	size_t i;

	if (dest == NULL || src == NULL)
		return (dest);
	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';

	return (dest);
}

char *_api_kstrcpy(char *dest, const char *src) __attribute__((alias("kstrcpy")));

