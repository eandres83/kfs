/* El entry point que esta definido en crt0.s */
ENTRY(_strat)

SECTIONS
{
	/* standar for user binaries on 32 bits */
	. = 0x08048000;
	.text ALIGN(4K) :
	{
		*(.txt)
		*(.rodata*)
	}

	.data ALIGN(4K) :
	{
		*(.data)
	}

	.bss ALIGN(4K) :
	{
		*(COMMON)
		*(.bss)
	}
}

