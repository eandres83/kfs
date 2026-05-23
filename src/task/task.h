#ifndef TASK_H
#define TASK_H

#include <utils.h>
#include <kmalloc.h>
#include "mm/vmm.h"
#include "mm/gdt.h"
#include "arch/i386/idt.h"
#include "fs/vfs/vfs.h"
#include "task/elf.h"
#include "fs/fd.h"
#include "drivers/tty/tty.h"

#define MAX_FD 64

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
	uint32_t	ruid;	// Real uid
	uint32_t	euid;	// Effective uid
	uint32_t	gid;	// Group id
	uint32_t	exit_status;
	uint32_t	signals;
	uint32_t	signal_handlers[32];
	uint32_t	mmap_count;
	uint32_t	mmap_allocation[32];
	char		*kstack;
	char		*user_stack;
	char		*user_eip;
	char		pwd[256];
	void		*pd;
	enum procstate	state;
	struct file	*fd_table[MAX_FD];
	struct proc 	*parent;
	struct context	*context;
	struct vfs_node	*node;
} proc_t;

extern void jump_to_usermode(uint32_t entry_point, uint32_t user_stack);
extern void swtch(struct context **old, struct context *new_proc);
extern void fork_child_exit();

void create_init_process();
void yield();
void find_signal(registers_t *regs);
void kill_process(char *motivo);

// syscall
void 	exit_process(uint32_t status);
ssize_t	wait(uint32_t *status);
ssize_t	waitpid(ssize_t pid, uint32_t *status, uint32_t options);
ssize_t setuid(uint32_t uid);
ssize_t getuid();
ssize_t setgid(uint32_t gid);
ssize_t	getgid();
ssize_t fork(registers_t *regs);
ssize_t kill(uint32_t pid, uint32_t signal);
ssize_t signal(uint32_t signum, void (*function));
ssize_t mmap(ssize_t size);
ssize_t munmap(void *addr, size_t size);
char	*getcwd(char *buf, size_t size);
ssize_t	chdir(char *path);

// helper for fs
proc_t	*get_current_process();
struct vfs_node *get_current_node();
void	set_new_node(struct vfs_node *new_node);
void	set_new_pwd(char *pwd);

void	set_current_process();

#endif
