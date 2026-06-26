#include "gdt.h"
#include "modules/modules.h"

struct gdt_entry_struct	*gdt = (struct gdt_entry_struct *)GDT_ADDRESS;
struct gdt_ptr_struct 	gdt_ptr;

static uint8_t tss_stack[4096];

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
	gdt_ptr.limit = (sizeof(struct gdt_entry_struct) * GDT_ENTRIES) - 1;
	gdt_ptr.base = (uint32_t)gdt;

	// gdt entry = 8 bytes
	gdt_set_gate(0, 0, 0, 0, 0);

	// Entrada 1. Kernel code
	gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

	// Entrada 2 Kernel data
	gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

	// Entrada 3 Kernel stack
	gdt_set_gate(3, 0, 0xFFFFFFFF, 0x92, 0xCF);

	// Entrada 4 User code
	gdt_set_gate(4, 0, 0xFFFFFFFF, 0xFA, 0xCF);

	// Entrada 5 User data
	gdt_set_gate(5, 0, 0xFFFFFFFF, 0xF2, 0xCF);

	// Entrada 6 User stack
	gdt_set_gate(6, 0, 0xFFFFFFFF, 0xF2, 0xCF);

	kmemset(&gdt_ptr.tss, 0, sizeof(struct tss_entry));
	gdt_ptr.tss.ss0 = 0x10; // kernel data segment
	gdt_ptr.tss.esp0 = (uint32_t)tss_stack + 4096;

	uint32_t base = (uint32_t)&gdt_ptr.tss;
	uint32_t limit = sizeof(struct tss_entry) - 1;

	// 0x89 TSS 32 bits type (1001)
	gdt_set_gate(7, base, limit, 0x89, 0x00);

	gdt_flush((uint32_t)&gdt_ptr);
	tss_flush(0x38);
}

void set_kernel_stack(uint32_t stack)
{
	gdt_ptr.tss.esp0 = stack;
}

