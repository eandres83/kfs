#include "../minilib.h"

void	putnbr(long n, int base)
{
	char buffer[32];
	int i;
	int is_neg;
	int remainder;

	i = 0;
	is_neg = 0;
	if (n == 0)
	{
		write(1, "0", 1);
		return ;
	}
	if (n < 0 && base == 10)
	{
		is_neg = 1;
		n = -n;
	}
	while (n != 0)
	{
		remainder = n % base;
		if (remainder > 9)
			buffer[i++] = (remainder - 10) + 'a';
		else
			buffer[i++] = remainder + '0';
		n /= base;
	}
	if (is_neg)
		buffer[i++] = '-';
	while (i > 0)
	{
		i--;
		printf("%c", buffer[i]);
	}
}

