#ifndef GDT_H
#define GDT_H

#define GDT_ENTRIES 9
#define GDT_ADDRESS 0xc0000800

#include <utils.h>
#include <uapi.h>
#include <stdint.h>

struct tss_entry
{
	uint32_t nothing;	// variable to respect the offset expected by the CPU (prev_tss)
	uint32_t esp0;		// The stack pointer to load when changing
	uint32_t ss0;		// The stack segment to load when changing
} __attribute__((packed));

struct gdt_entry_struct
{
	uint16_t	limit_low;	// Los 16 bits abajo de limite
	uint16_t	base_low;	// Los 16 bits bajos de la base
	uint8_t		base_middle;	// Los siguientes 8 bits de la base
	uint8_t		access;		// Byte de Acceso
	uint8_t		granularity;	// Granularidad + 4 bits altos del limite
	uint8_t		base_high;	// Los ultimos 8 bits de la base
} __attribute__((packed));

struct	gdt_ptr_struct
{
	uint16_t		limit;
	uint32_t		base;	// Direccion de memoria donde empieza GDT
	struct tss_entry	tss;
} __attribute__((packed));

void	init_gdt();

void set_kernel_stack(uint32_t stack);

extern void gdt_flush(uint32_t);
extern void tss_flush(uint16_t);

#endif
