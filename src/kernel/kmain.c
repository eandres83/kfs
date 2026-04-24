#include <kmalloc.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "arch/i386/idt.h"
#include "arch/i386/timer.h"
#include "mm/slab.h"
#include "task/task.h"
#include "fs/vfs/vfs.h"
#include "fs/ext2/ext2.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* Only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This code needs to be compiled with a ix86-elf compiler"
#endif

extern uint32_t	get_stack_pointer();
//extern void	prompt();

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

void	cat()
{
	char *path = "/home/kfs/texto.txt";
	struct vfs_node *node = get_vfs_node_path(path);
	if (node == NULL || node->type != VFS_FILE)
		return ;
	node->ops->read(node);
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
	init_idt();
	vmm_initialize();

	init_kmalloc();

	kprintf("Multiboot info address: 0x%x\n", (uint32_t)boot_info);
	kprintf("Memory map address: 0x%x\n", boot_info->mmap_addr);

	init_keyboard();
//	init_timer(1000);

	init_vfs();
	init_ext2();

	kprintf("KFS> ");
	cat();
//	iniciar_multitarea();

	asm volatile ("sti");
	while (1)
		asm volatile ("hlt");

//	prompt();
}

