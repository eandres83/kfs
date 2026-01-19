#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/vga.h"
#include "drivers/keyboard.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This code needs to be compiled with a ix86-elf compiler"
#endif

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

extern uint32_t	get_stack_pointer();

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
	terminal_setcolor(VGA_COLOR_LIGHT_GREY);
}

void	kernel_main(void)
{
	/* Initialize terminal interface */
	terminal_initialize();
	print_splash();

	terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
	terminal_writestring("42\n");

	void *esp = (void*)get_stack_pointer();
	print_stack(esp, 10);

	while (1)
	{
		char c = keyboard_read_char();
		if (c > 0)
			terminal_putchar(c);
	}

}

