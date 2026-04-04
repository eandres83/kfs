#ifndef PROCESS_H
#define PROCESS_H

#include <utils.h>
#include <kmalloc.h>
#include "../mm/vmm.h"
#include "../mm/gdt.h"

enum procstate { UNUSED, EMBRYO, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct context
{
	uint32_t edi;
	uint32_t esi;
	uint32_t ebx;
	uint32_t ebp;
	uint32_t eip;
};

typedef struct proc
{
	uint32_t	id;
	uint32_t	pid;
	char		*kstack;
	char		*user_stack;
	char		*user_eip;
	enum procstate	state;
	struct proc 	*parent;
	struct context	*context;
} proc_t;

struct tss_entry
{
	uint32_t prev_tss;	// The previous TSS
	uint32_t esp0;		// The stack pointer to load when changing
	uint32_t ss0;		// The stack segment to load when changing
} __attribute__((packed));

extern void jump_to_usermode(uint32_t entry_point, uint32_t user_stack);
extern void swtch(struct context **old, struct context *new_proc);

void test_ring3();
void iniciar_multitarea();
void yield();

#endif
