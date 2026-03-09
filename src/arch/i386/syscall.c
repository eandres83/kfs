#include "arch/i386/idt.h"

void	sys_write(registers_t *regs)
{
	(void)regs;
	kprintf("write syscall called\n");
}

void	sys_read(registers_t *regs)
{
	(void)regs;
	kprintf("read syscall called\n");
}

static void	(*syscall[10])(registers_t*) =
{
	[3] = sys_read,
	[4] = sys_write
};

void 	syscall_callback(registers_t *regs)
{
	kprintf("Syscall %d requested!\n", regs->eax);

	// check if regs->eax (syscall number) exists
	if (regs->eax < 10 && syscall[regs->eax] != NULL)
		syscall[regs->eax](regs);
	else
		kprintf("Error: Syscall not implemented.\n");
}

