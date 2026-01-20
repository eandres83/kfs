#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/vga.h"
#include "gdt.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This code needs to be compiled with a ix86-elf compiler"
#endif

extern uint32_t	get_stack_pointer();
extern void	prompt();

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

void	kernel_main(void)
{
	init_gdt();
	/* Initialize terminal interface */
	terminal_initialize();
	print_splash();

	prompt();
}

