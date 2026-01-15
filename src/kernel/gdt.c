#include "gdt.h"

void	gdt_set_gate(

void	init_gdt()
{
	struct gdt_entry_struct *gdt = (struct gdt_entry_struct *)0x00000800;

	struct gdt_ptr_struct gp;
	gp.limit = (sizeof(struct gdt_entry_struct) * NUM_ENTRIES) - 1;
	gp.base = 0x00000800;

	// Escribir en GDT, lo primero todo NULL
	gdt_set_gate(0, 0, 0, 0, 0);

	// Entrada 1. Kernel code
	gdt_set_gate();
}

