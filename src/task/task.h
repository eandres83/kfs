#ifndef PROCESS_H
#define PROCESS_H

#include <utils.h>
#include <kmalloc.h>
#include "../mm/vmm.h"

extern void jump_to_usermode(uint32_t entry_point, uint32_t user_stack);

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

//typedef struct registers
//{
//	uint32_t edi;
//	uint32_t esi;
//	uint32_t ebx;
//	uint32_t ebp;
//	uint32_t eip;
//
//	uint32_t cs;
//	uint32_t esp;
//} registers_t;

typedef struct proc
{
	uint32_t	id;
	uint32_t	pid;
	char		*kstack;
	enum procstate	state;
	struct proc 	*parent;
//	struct registers *regs;
} proc_t;

struct tss_entry
{
	uint32_t prev_tss;	// The previous TSS
	uint32_t esp0;		// The stack pointer to load when changing
	uint32_t ss0;		// The stack segment to load when changing
} __attribute__((packed));

void test_ring3();

#endif
