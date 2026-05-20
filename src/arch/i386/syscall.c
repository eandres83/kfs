#include "arch/i386/idt.h"
#include "task/task.h"
#include "task/elf.h"
#include "arch/i386/lib/uaccess.h"
#include "fs/sysfile.h"

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

ssize_t sys_open(registers_t *regs)
{
	return (open((char*)regs->ebx, regs->ecx, regs->edx));
}

ssize_t sys_close(registers_t *regs)
{
	return (close(regs->ebx));
}

ssize_t sys_waitpid(registers_t *regs)
{
	waitpid(regs->ebx, &regs->ecx, regs->edx);
	return (regs->ecx);
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

ssize_t	sys_access(registers_t *regs)
{
	return (access((char *)regs->ebx, regs->ecx));
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

ssize_t sys_dup(registers_t *regs)
{
	return (dup(regs->ebx));
}

ssize_t sys_pipe(registers_t *regs)
{
	return (pipe((int*)regs->ebx));
}

ssize_t sys_signal(registers_t *regs)
{
	return (signal(regs->ebx, (void (*)())regs->ecx));
}

ssize_t	sys_dup2(registers_t *regs)
{
	return (dup2(regs->ebx, regs->ecx));
}

ssize_t sys_mmap(registers_t *regs)
{
	return (mmap(regs->ebx));
}

ssize_t sys_munmap(registers_t *regs)
{
	return (munmap((void*)regs->ebx, regs->ecx));
}

ssize_t	sys_getcwd(registers_t *regs)
{
	return ((ssize_t)getcwd((char*)regs->ebx, regs->ecx));
}

static ssize_t	(*syscall[200])(registers_t*) =
{
	[SYS_exit] 	= sys_exit,
	[SYS_fork] 	= sys_fork,
	[SYS_read] 	= sys_read,
	[SYS_write] 	= sys_write,
	[SYS_open] 	= sys_open,
	[SYS_close] 	= sys_close,
	[SYS_waitpid]	= sys_waitpid,
	[SYS_wait] 	= sys_wait,
	[SYS_execve] 	= sys_execve,
	[SYS_getuid] 	= sys_getuid,
	[SYS_access] 	= sys_access,
	[SYS_kill] 	= sys_kill,
	[SYS_dup] 	= sys_dup,
	[SYS_pipe] 	= sys_pipe,
	[SYS_signal] 	= sys_signal,
	[SYS_dup2] 	= sys_dup2,
	[SYS_mmap] 	= sys_mmap,
	[SYS_munmap]	= sys_munmap,
	[SYS_getcwd] 	= sys_getcwd
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

