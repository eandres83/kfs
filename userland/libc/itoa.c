#include "../minilib.h"

static int count_size(int n)
{
	int i = 0;

	if (n < 0)
		n *= -1;
	while (n != 0)
	{
		n /= 10;
		i++;
	}
	return (i);
}

static char *strnew(size_t size)
{
	char *str;

	str = (char*)malloc(sizeof(*str) * (size + 1));
	if (!str)
		return (NULL);
	bzero(str, size + 1);
	return (str);
}

char	*itoa(int num)
{
	char	*dest;
	int	count = count_size(num);
	int	i = 0;

	if (num < 0 || count == 0)
		count++;
	dest = strnew(count);
	if (dest == NULL)
		return (NULL);
	if (num < 0)
	{
		num *= -1;
		dest[0] = '-';
		i++;
	}
	while (count-- > i)
	{
		dest[count] = (num % 10) + '0';
		num /= 10;
	}
	return (dest);
}

