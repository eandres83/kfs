#include <utils.h>
#include "drivers/vga.h"

void	kprintf(const char *format, ...)
{
	va_list args;
	va_start(args, format);

	for (int i = 0; format[i] != '\0'; i++)
	{
		if (format[i] == '%')
		{
			i++;
			if (format[i] == 'c')
				terminal_putchar((char)va_arg(args, int));
			else if (format[i] == 's')
				terminal_writestring(va_arg(args, char*));
			else if (format[i] == 'd')
				kputnbr((long long)va_arg(args, int), 10);
			else if (format[i] == 'x')
				kputnbr((long long)va_arg(args, unsigned int), 16);
			else if (format[i] == 'p')
			{
				terminal_writestring("0x");
				kputnbr((unsigned long)va_arg(args, void*), 16);
			}
			else if (format[i] == '%')
				terminal_putchar('%');
		}
		else
			terminal_putchar(format[i]);
	}
	va_end(args);
}

