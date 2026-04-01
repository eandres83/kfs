#ifndef GDT_H
#define GDT_H

#define GDT_ENTRIES 8
#define GDT_ADDRESS 0x00000800

#include <utils.h>
#include <stdint.h>
#include "../task/task.h"

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

extern void gdt_flush(uint32_t);
extern void tss_flush(uint16_t);

#endif
