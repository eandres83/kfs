#include "arch/i386/idt.h"
#include "task/task.h"
#include "task/elf.h"
#include "arch/i386/lib/uaccess.h"

ssize_t sys_exit(registers_t *regs)
{
	exit_process(regs->ebx);
	// medida de seguridad por si llegase hasta aqui
	while (1)
		asm volatile ("hlt");
	return (1); // nunca va a llegar ;)
}

ssize_t sys_fork(registers_t *regs)
{
	return (fork(regs));
}

ssize_t	sys_read(registers_t *regs)
{
	return (read(regs->ebx, (void*)regs->ecx, regs->edx));
}

ssize_t	sys_write(registers_t *regs)
{
	return (write(regs->ebx, (const char*)regs->ecx, regs->edx));
}

ssize_t sys_wait(registers_t *regs)
{
	wait(&regs->ebx);
	return (regs->ebx);
}

ssize_t sys_execve(registers_t *regs)
{
	ssize_t res = execve((char*)regs->ebx, (char**)regs->ecx, (char**)regs->edx, regs);
	if (res == -1)
		kprintf("Error: something wrong in execve :(\n");
	return (res);
}

ssize_t sys_getuid(registers_t *regs)
{
	(void)regs;
	return (getuid());
}

ssize_t sys_kill(registers_t *regs)
{
	return (kill(regs->ebx, regs->ecx));
}

ssize_t sys_signal(registers_t *regs)
{
	return (signal(regs->ebx, (void (*)())regs->ecx));
}

ssize_t sys_mmap(registers_t *regs)
{
	return (mmap(regs->ebx));
}

ssize_t sys_munmap(registers_t *regs)
{
	return (munmap((void*)regs->ebx, regs->ecx));
}

static ssize_t	(*syscall[200])(registers_t*) =
{
	[1] = sys_exit,
	[2] = sys_fork,
	[3] = sys_read,
	[4] = sys_write,
	[7] = sys_wait,
	[11] = sys_execve,
	[24] = sys_getuid,
	[37] = sys_kill,
	[48] = sys_signal,
	[90] = sys_mmap,
	[91] = sys_munmap
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

