#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/vga.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void	kernel_main(void)
{
	/* Initialize terminal interface */
	terminal_initialize();

	terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
	terminal_writestring("42\n");

	terminal_setcolor(VGA_COLOR_LIGHT_GREEN);

	int i = 0;
	while (i < 50)
	{
		terminal_writestring("Probando scroll... linea de relleno\n");
		i++;
	}
	terminal_setcolor(VGA_COLOR_WHITE);
	kprintf("Hola esto es una prueba para ver que mi printf, \n y esto es el valor de un int -> %d", i);
}

