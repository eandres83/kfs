#include <utils.h>
#include "drivers/vga.h"

void	panic(char *msg, const char *file, uint32_t line)
{
	terminal_setcolor(VGA_COLOR_RED);
	asm volatile("cli");


	terminal_initialize();

	terminal_writestring("\n\n*** KERNEL PANIC ***\n\n");

	kprintf("Message: %s\n", msg);
	kprintf("File: %s\n", file);
	kprintf("Line: %d\n", line);

	terminal_writestring("System Halted.");

	while (1)
		asm volatile("hlt");
}

