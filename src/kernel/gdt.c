#include "gdt.h"

struct gdt_entry_struct	gdt[GDT_ENTRIES];
struct gdt_ptr_struct 		*gdt_ptr = (struct gdt_ptr_struct *)GDT_ADDRESS;

static void	gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
	// base address configuration
	gdt[num].base_low =  (base & 0xFFFF); // save first 16 bits
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	// limit configuration
	gdt[num].limit_low = (limit & 0xFFFF); // save first 16 bits
	gdt[num].granularity = (limit >> 16) & 0x0F;

	gdt[num].access = access;
	gdt[num].granularity |= (gran & 0xF0);
}

void	init_gdt()
{
	gdt_ptr.limit = (sizeof(struct gdt_ptr_struct) * GDT_ENTRIES) - 1;
	gdt_ptr.base = (uint32_t)gdt;

	// Escribir en GDT, lo primero todo NULL
	gdt_set_gate(0, 0, 0, 0, 0);

	// Entrada 1. Kernel code
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

	// Entrada 2 Kernel data
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xC);

	// Entrada 3 Kernel stack
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0x92, 0xC);

	// Entrada 4 User code
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xFA, 0xC);

	// Entrada 5 User data
	gdt_set_gate(5, 0, 0xFFFFFFFF, 0xF2, 0xC);

	// Entrada 6 User stack
	gdt_set_gate(6, 0, 0xFFFFFFFF, 0xF2, 0xC);

	gdt_flush((uint32_t)&gdt_ptr);
}

