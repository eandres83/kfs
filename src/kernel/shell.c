#include <utils.h>
#include <kmalloc.h>
#include "drivers/keyboard.h"
#include "drivers/vga.h"
#include "../drivers/io.h"
#include "mm/gdt.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "task/task.h"

#define BUFFER_SIZE 256

static char buffer[BUFFER_SIZE];
static uint32_t index = 0;

void	test_malloc_free()
{
	kprintf("Test SMALL\n");
	char *str1 = (char*)kmalloc(32);
	if (str1)
	{
		str1[0] = 'H';
		str1[1] = 'o';
		str1[2] = 'l';
		str1[3] = 'a';
		str1[4] = '\n';

		kprintf("Direccion: %p | Contenido: %s", str1, str1);
	}

	kprintf("Test SMALL (512)");
	char *str2 = (char*)kmalloc(512);
	kprintf("Direccion: %p\n", str2);
 
	kprintf("Test LARGE (8192)");
	char *str3 = (char*)kmalloc(8192);
	kprintf("Direccion: %p\n", str3);

	kprintf("Liberando memoria...\n");
	kfree(str1);
	kfree(str2);
	kfree(str3);

	kprintf("Test ksize\t");
	void *ptr = kmalloc(20);
	size_t size = ksize(ptr);
	kprintf("size -> %d\n", size);
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

static inline void halt()
{
	asm volatile("hlt");
}

static void reboot()
{
	uint8_t good = 0x02;
	while (good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	halt();
}

static void shutdown()
{
	// Se utiliza outw porque son 16 bits no 8
	outw(0x604, 0x2000);
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
		reboot();
	}
	else if (kstrcmp(str, "halt") == 0)
	{
		kprintf("Stopping CPU. Bye!\n");
		halt();
	}
	else if (kstrcmp(str, "shutdown") == 0)
	{
		kprintf("Shutdown now");
		shutdown();
	}
	else if (kstrcmp(str, "clear") == 0)
	{
		terminal_initialize();
	}
	else if (kstrcmp(str, "malloc_test") == 0)
	{
		terminal_setcolor(VGA_COLOR_RED);
		test_malloc_free();
		terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
	}
	else if (kstrcmp(str, "meminfo") == 0)
	{
		meminfo();
	}
	else if (kstrcmp(str, "virt2phys") == 0)
	{
		// 0xC0400000, 0xC0100000
		virt2phys(0xC010010A);
	}
	else if (kstrcmp(str, "interrupt") == 0)
	{
		asm volatile ("int $0x3");
	}
	else if (kstrcmp(str, "layout fr") == 0)
	{
		set_keyboard_layout("fr");
	}
	else if (kstrcmp(str, "layout us") == 0)
	{
		set_keyboard_layout("us");
	}
	else if (kstrcmp(str, "test_syscall") == 0)
	{
		asm volatile ("int $0x80" : : "a"(4));
	}
	else if (kstrcmp(str, "ring3") == 0)
	{
		test_ring3();
	}
	else if (kstrlen(str) > 0)
		kprintf("Unknown command: %s\n", str);
}

void	shell_handle_keypress(char c)
{
	terminal_setcolor(VGA_COLOR_LIGHT_CYAN);

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

