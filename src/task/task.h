#ifndef TASK_H
#define TASK_H

#include <utils.h>
#include <kmalloc.h>
#include "mm/vmm.h"
#include "mm/gdt.h"
#include "arch/i386/idt.h"

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
	// id es inutil, no la uso porque no utilizo aceso directo a
	// procesos de array, comprobar si la uso antes de entregar
	uint32_t	id;
	uint32_t	pid;
	uint32_t	uid;
	uint32_t	exit_status;
	uint32_t	signals;
	uint32_t	signal_handlers[32];
	uint32_t	mmap_count;
	uint32_t	mmap_allocation[32];
	char		*kstack;
	char		*user_stack;
	char		*user_eip;
	void		*pd;
	enum procstate	state;
	struct proc 	*parent;
	struct context	*context;
} proc_t;

extern void jump_to_usermode(uint32_t entry_point, uint32_t user_stack);
extern void swtch(struct context **old, struct context *new_proc);
extern void fork_child_exit();

void iniciar_multitarea();
void yield();
void find_signal(registers_t *regs);
void kill_process(char *motivo);

// syscall
void 	exit_process(uint32_t status);
ssize_t	wait(uint32_t *status);
ssize_t getuid();
ssize_t fork(registers_t *regs);
ssize_t kill(uint32_t pid, uint32_t signal);
ssize_t signal(uint32_t signum, void (*function));
ssize_t mmap();
ssize_t munmap(void *addr);

// temp para kfs-5 borrar
ssize_t sendmsg(uint32_t pid, char *msg, uint32_t len);
ssize_t recvmsg(char *dest, size_t len);
__attribute__((section(".user_text"))) void proceso_test_syscall();

#endif
