/* El entry point que esta definido en crt0.s */
ENTRY(_start)

SECTIONS
{
	/* standar for user binaries on 32 bits */
	. = 0x08048000 + SIZEOF_HEADERS;
	.text ALIGN(4K) :
	{
		*(.text)
		*(.rodata*)
	}

	.data ALIGN(4K) :
	{
		*(.data)
	}

	.bss :
	{
		*(COMMON)
		*(.bss)
	}
}

