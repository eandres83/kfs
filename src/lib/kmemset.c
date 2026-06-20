#include <utils.h>

void	*kmemset(void *dest, int c, size_t n)
{
	char	*temp_dest;

	temp_dest = (char *)dest;
	while (n > 0)
	{
		*temp_dest = c;
		temp_dest++;
		n--;
	}
	return (dest);
}

void *_api_kmemset(void *dest, int c, size_t n) __attribute__((alias("kmemset")));

