#include <kmalloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "mm/gdt.h"
#include "mm/pmm.h"
#include "mm/vmm.h"

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
	const char *logo[] = {
	"  _  __  _____   ____  ",
	" | |/ / |  ___| / ___| ",
	" | ' /  | |_    \\___ \\ ",
	" | . \\  |  _|    ___) |",
	" |_|\\_\\ |_|     |____/ ",
	0
	};

	terminal_writestring("\n");

	for (int row = 0; logo[row]; row++)
	{
		for (int col = 0; logo[row][col]; col++)
		{
			terminal_setcolor(((col + row) % 14) + 1);
			terminal_putchar(logo[row][col]);
		}
		terminal_putchar('\n');
	}
	terminal_writestring("\n");

	terminal_setcolor(VGA_COLOR_LIGHT_MAGENTA);
	terminal_writestring(" By eandres - KFS v0.1\n\n");

	terminal_setcolor(VGA_COLOR_LIGHT_CYAN);
}

static	void	test_malloc_free()
{
	char *str1 = (char*)kmalloc(32);
	if (str1)
	{
		str1[0] = 'H';
		str1[1] = 'o';
		str1[2] = 'l';
		str1[3] = 'a';
		str1[4] = '\n';

		kprintf("Direccion: %p | Contenido: %s\n", str1, str1);
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

	kprintf("Reutilizar memoria (32 bytes)\n");
	char *str4 = (char *)kmalloc(32);
	kprintf("Nueva direccion: %p (Igual que la TINY)\n");
	kfree(str4);
}

void	kernel_main(uint32_t magic, multiboot_info_t *boot_info)
{
	/* Initialize terminal interface */
	terminal_initialize();
	print_splash();

	// Make sure the magic number matches for memory mapping
	if (magic != 0x2BADB002)
		kprintf("error");

	init_pmm(boot_info);

	init_gdt();
	vmm_initialize();

	kprintf("Multiboot info address: 0x%x\n", (uint32_t)boot_info);
	kprintf("Memory map address: 0x%x\n", boot_info->mmap_addr);

	init_keyboard();

	test_malloc_free();

	prompt();
}

