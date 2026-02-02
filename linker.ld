/* The bootloader will look at this image ans start execution at the symbol
   designated as the entry point. */
ENTRY(_start)

/* Tell where the various sections of the object files will be put in the final
   kernel image. */
SECTIONS
{
	. = 0x00200000;
	/* The kernel will live at 3GB + 2MB in the virtual address space,
	   which will be mapped to 1MB in the physical address space.
	   Note that we page-align the sectiong. */

	_kernel_start = .;

	/* ADDR(.text) is the virtual direction (0xC0200000).
	   We subtract 0xC0000000 to obtain the physical address (0x00200000).
	   AT(...) is for virtual memory */
	.text : AT(ADDR(.text) - 0xC0000000)
	{
		*(.multiboot)
		*(.text)
	}
	
	/* Read-only data. */
	.rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xC0000000)
	{
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss ALIGN(4K) : AT(ADDR(.data) - 0xC0000000)
	{
		*(COMMON)
		*(.bss)
	}

	_end = .;

	/* The compiler may produce other sections, by default it will put them in
	   a segment with the same name. Simply and stuff here as needed. */
}

