#include <utils.h>

void	kputnbr(long long n, int base)
{
	char buffer[32];
	int i;
	int is_neg;
	int remainder;

	i = 0;
	is_neg = 0;
	if (n == 0)
	{
		terminal_putchar('0');
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
		buf[i++] = '-';

	while (i > 0)
	{
		i--;
		terminal_putchar(buffer[i]);
	}
}

