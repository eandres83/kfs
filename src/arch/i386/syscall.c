#include "arch/i386/idt.h"

ssize_t	sys_write(registers_t *regs)
{
	if (regs->ebx != 1)
	{
		kprintf("Error: bad file descriptor\n");
		return (-1);
	}
	const char *buf = (char*)regs->ecx;
	size_t count = regs->edx;

	terminal_write(buf, count);
	return (count);
}

ssize_t	sys_read(registers_t *regs)
{
	(void)regs;
	kprintf("read syscall called\n");
	return (-1);
}

static ssize_t	(*syscall[10])(registers_t*) =
{
	[3] = sys_read,
	[4] = sys_write
};

void 	syscall_callback(registers_t *regs)
{
//	kprintf("Syscall %d requested!\n", regs->eax);

	// check if regs->eax (syscall number) exists
	if (regs->eax < 10 && syscall[regs->eax] != NULL)
		regs->eax = syscall[regs->eax](regs);
	else
		kprintf("Error: Syscall not implemented.\n");
}

