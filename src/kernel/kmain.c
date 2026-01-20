#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "drivers/io.h"
#include "gdt.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This code needs to be compiled with a ix86-elf compiler"
#endif

#define BUFFER_SIZE 256

extern uint32_t	get_stack_pointer();

static void	print_splash()
{
	terminal_setcolor(VGA_COLOR_LIGHT_GREEN);
	terminal_writestring("\n");
	terminal_writestring("  _  __  _____   ____  \n");
	terminal_writestring(" | |/ / |  ___| / ___| \n");
	terminal_writestring(" | ' /  | |_	\\___ \\ \n");
	terminal_writestring(" | . \\  |  _|	___) |\n");
	terminal_writestring(" |_|\\_\\ |_|	 |____/ \n");
	terminal_writestring("\n");
	
	terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
	terminal_writestring("   By eandres - KFS v0.1\n\n");
}

static	void	print_stack(void *stack_ptr, size_t lines)
{
	unsigned char *ptr;

	ptr = (unsigned char *)stack_ptr;

	kprintf("Stack Dump: \n");

	for (size_t i = 0; i < lines; i++)
	{
		terminal_setcolor(VGA_COLOR_BLUE);
		kprintf("%p : ", ptr);

		terminal_setcolor(VGA_COLOR_RED);
		for (int x = 0; x < 16; x++)
		{
			if (ptr[x] < 16)
				terminal_putchar('0');
			kprintf("%x ", ptr[x]);
		}
		terminal_setcolor(VGA_COLOR_GREEN);
		terminal_writestring(" |");

		for (int j = 0; j < 16; j ++)
		{
			unsigned char c = ptr[j];

			if (c >= 32 && c <= 126)
				terminal_putchar(c);
			else
				terminal_putchar('.');
		}
		terminal_writestring("|");

		terminal_writestring("\n");
		ptr += 16;
	}
	terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
}

static void	execute_command(char *str)
{
	if (kstrcmp(str, "stack") == 0)
	{
		print_stack((void *)0x800, 4);
	}
	else if (kstrcmp(str, "reboot") == 0)
	{
		kprintf("Rebooting... \n");
		outb(0x64, 0xFE);
	}
	else if (kstrcmp(str, "halt") == 0)
	{
		kprintf("Stopping CPU. Bye!\n");
		asm volatile("hlt");
	}
	else if (kstrlen(str) > 0)
		kprintf("Unknown command: %s\n", str);
}

void	kernel_main(void)
{
	init_gdt();
	/* Initialize terminal interface */
	terminal_initialize();
	print_splash();

	terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
	kprintf("KFS> ");

	char	buffer[BUFFER_SIZE];
	int	index = 0;

	while (1)
	{
		char c = keyboard_read_char();
		if (c > 0)
		{
			if (c == '\n')
			{
				terminal_putchar('\n');
				buffer[index] = '\0';
				execute_command(buffer);

				index = 0;
				kmemset(buffer, 0, BUFFER_SIZE);
				kprintf("KFS> ");
			}
			else if (c == '\b')
			{
				if (index > 0)
				{
					index--;
					terminal_putchar('\b');
					terminal_putchar(' ');
					terminal_putchar('\b');
				}
			}
			else
			{
				if (index < BUFFER_SIZE - 1)
				{
					buffer[index] = c;
					index++;
					terminal_putchar(c);
				}
			}
		}
	}
}

