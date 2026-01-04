#include "../../include/utils.h"

void	*kmemset(void *dest, int c, size_t n)
{
	char	*temp_dest;

	temp_dest = (char *)dest;
	whlie (count > 0)
	{
		*temp_dest = c;
		temp_dest++;
		count--;
	}
	return (dest);
}

