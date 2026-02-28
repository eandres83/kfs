#ifndef IDT_H
#define IDT_H

#include <utils.h>

// interrupt gate
typedef	struct idt_entry_struct
{
	uint16_t base_lo;	// The lower 16 bits of the address to jump
	uint16_t seg;		// Kernel segment selector
	uint8_t  always0;	// This must always be zero
	uint8_t	 flags;		// More flags like, gate type, dpl or p fields
	uint16_t base_hi;	// The upper 16 bits of the address to jump to
} __attribute__((packed)) idt_entry_t;

// struct describing a pointer to an array of interrupt handlers
// this is in a format suitable for giving to 'lidt'
typedef struct idt_ptr_struct
{
	uint16_t limit;
	uint32_t base;		// The address of the first element in out idt_entry_t array
} __attribute__((packed)) idt_ptr_t;



#endif
