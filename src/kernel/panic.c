#include <utils.h>
#include "drivers/vga.h"

void	panic(char *msg, const char *file, uint32_t line)
{
	asm volatile("cli");

	terminal_setcolor(VGA_COLOR_RED);

	terminal_initialize();

	terminal_writestring("\n\n*** KERNEL PANIC ***\n\n");

	kprintf("Message: %s\n", msg);
	kprintf("File: %s\n", file);
	kprintf("Line: %d\n", line);

	terminal_writestring("System Halted.");

	while (1)
		asm volatile("hlt");
}

