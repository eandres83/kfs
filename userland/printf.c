#include "minilib.h"

static void	print_string(char *str)
{
	uint32_t i = 0;

	while (str[i])
	{
		write(1, &str[i], 1);
		i++;
	}
}

void	printf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	for (int i = 0; format[i] != '\0'; i++)
	{
		if (format[i] == '%')
		{
			i++;
			if (format[i] == 'c')
				write(1, &format[i], 1);
			else if (format[i] == 's')
				print_string(va_arg(args, char*));
			else if (format[i] == 'd')
				putnbr((long long)va_arg(args, int), 10);
			else if (format[i] == 'x')
				putnbr((long long)va_arg(args, unsigned int), 16);
			else if (format[i] == 'p')
			{
				write(1, "0x", 2);
				putnbr((unsigned long)va_arg(args, void*), 16);
			}
			else if (format[i] == '%')
				write(1, "%", 1);
		}
		else
			write(1, &format[i], 1);
	}
	va_end(args);
}

