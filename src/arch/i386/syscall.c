#include "arch/i386/idt.h"
#include "task/task.h"

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

ssize_t sys_exit(registers_t *regs)
{
	exit_process(regs->ebx);
	// medida de seguridad por si llegase hasta aqui
	while (1)
		asm volatile ("hlt");
	return (1); // nunca va a llegar
}

ssize_t sys_wait(registers_t *regs)
{
	wait(&regs->ebx);
	return (regs->ebx);
}

ssize_t sys_getuid(registers_t *regs)
{
	(void)regs;
	return (getuid());
}

ssize_t sys_fork(registers_t *regs)
{
	return (fork(regs));
}

static ssize_t	(*syscall[200])(registers_t*) =
{
	[3] = sys_read,
	[4] = sys_write,
	[57] = sys_fork,
	[60] = sys_exit,
	[61] = sys_wait,
	[102] = sys_getuid
};

void 	syscall_callback(registers_t *regs)
{
//	kprintf("Syscall %d requested!\n", regs->eax);

	// check if regs->eax (syscall number) exists
	if (regs->eax < 200 && syscall[regs->eax] != NULL)
		regs->eax = syscall[regs->eax](regs);
	else
		kprintf("Error: Syscall not implemented.\n");
}

