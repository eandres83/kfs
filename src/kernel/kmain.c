#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "arch/i386/idt.h"
#include "arch/i386/timer.h"
#include "mm/slab.h"
#include "mm/gdt.h"
#include "task/task.h"

#include "modules/modules.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This code needs to be compiled with a ix86-elf compiler"
#endif

extern uint32_t	get_stack_pointer();

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

void	kernel_main(uint32_t magic, multiboot_info_t *boot_info)
{
	/* Initialize terminal interface */
	terminal_initialize();
	print_splash();

	// Make sure the magic number matches for memory mapping
	if (magic != 0x2BADB002)
		kdebug("error");

	init_pmm(boot_info);

	init_gdt();
	init_idt();
	vmm_initialize();

	init_kmalloc();

	kdebug("Multiboot info address: 0x%x\n", (uint32_t)boot_info);
	kdebug("Memory map address: 0x%x\n", boot_info->mmap_addr);

	init_keyboard();
//	init_timer(1000);

	init_vfs();
//	create_init_process();

	ssize_t res = insmod("/bin/dummy_module.ko");
	if (res == -1)
		kprintf("Puto error en el insmod\n");
	else
		kprintf("Todo bien con el insmod, res == %d\n", res);

	kprintf("Ahora me lo reviento ;)\n");
	ssize_t res2 = rmmod("dummy_module");
	if (res2 == -1)
		kprintf("Puta mierda con el rmmod; res -> %d\n", res2);
	else
		kprintf("everything good\n");

	kprintf("\n\n");
	ssize_t res3 = insmod("/bin/dummy_module.ko");
	if (res3 == -1)
		kprintf("Puto error en el insmod\n");
	else
		kprintf("Todo bien con el insmod, res == %d\n", res3);

	asm volatile ("sti");
	while (1)
		asm volatile ("hlt");
}

