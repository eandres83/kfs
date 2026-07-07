#include "arch/i386/idt.h"
#include "arch/i386/gdt.h"
#include "task/task.h"
#include "task/elf.h"
#include "arch/i386/lib/uaccess.h"
#include "fs/sysfile.h"
#include "ipc/socket.h"
#include "modules/modules.h"

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
	uint32_t status;
	ssize_t pid = waitpid(regs->ebx, &status, regs->edx);
	if (!check_user_addr(regs->ecx))
		return (-1);
	*(uint32_t*)regs->ecx = status;
	return (pid);
}

ssize_t sys_wait(registers_t *regs)
{
	uint32_t status;
	ssize_t pid = wait(&status);
	if (!check_user_addr(regs->ebx))
		return (-1);
	*(uint32_t*)regs->ebx = status;
	return (pid);
}

ssize_t sys_execve(registers_t *regs)
{
	ssize_t res = execve((char*)regs->ebx, (char**)regs->ecx, (char**)regs->edx, regs);
	if (res == -1)
		kdebug("Error: something wrong in execve :(\n");
	return (res);
}

ssize_t	sys_chdir(registers_t *regs)
{
	return (chdir((char*)regs->ebx));
}

ssize_t	sys_getpid(registers_t *regs)
{
	(void)regs;
	return (getpid());
}

ssize_t	sys_access(registers_t *regs)
{
	return (access((char *)regs->ebx, regs->ecx));
}

ssize_t	sys_setuid(registers_t *regs)
{
	return (setuid(regs->ebx));
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

ssize_t	sys_brk(registers_t *regs)
{
	return (brk(regs->ebx));
}

ssize_t	sys_setgid(registers_t *regs)
{
	return (setgid(regs->ebx));
}

ssize_t	sys_getgid(registers_t *regs)
{
	(void)regs;
	return (getgid());
}

ssize_t sys_signal(registers_t *regs)
{
	return (signal(regs->ebx, (void (*)())regs->ecx));
}

ssize_t	sys_dup2(registers_t *regs)
{
	return (dup2(regs->ebx, regs->ecx));
}

ssize_t	sys_symlink(registers_t *regs)
{
	(void)regs;
	return (-1);
}

ssize_t	sys_readlink(registers_t *regs)
{
	(void)regs;
	return (-1);
}

ssize_t sys_mmap(registers_t *regs)
{
	return (mmap(regs->ebx));
}

ssize_t sys_munmap(registers_t *regs)
{
	return (munmap((void*)regs->ebx, regs->ecx));
}

ssize_t	sys_stat(registers_t *regs)
{
	return (stat_func((const char *)regs->ebx, (struct stat*)regs->ecx));
}

ssize_t	sys_lstat(registers_t *regs)
{
	return (lstat_func((const char *)regs->ebx, (struct stat*)regs->ecx));
}

ssize_t	sys_fstat(registers_t *regs)
{
	return (fstat_func(regs->ebx, (struct stat*)regs->ecx));
}

ssize_t	sys_init_module(registers_t *regs)
{
	return (insmod((char*)regs->ebx));
}

ssize_t	sys_del_module(registers_t *regs)
{
	return (rmmod((char*)regs->ebx));
}

ssize_t	sys_getdents(registers_t *regs)
{
	return (getdents(regs->ebx, (struct linux_dirent*)regs->ecx, regs->edx));
}

ssize_t	sys_getcwd(registers_t *regs)
{
	return ((ssize_t)getcwd((char*)regs->ebx, regs->ecx));
}

ssize_t	sys_socketpair(registers_t *regs)
{
	return (socketpair(regs->ebx, regs->ecx, regs->edx, (int*)regs->esi));
}

ssize_t	sys_sendmsg(registers_t *regs)
{
	return (sendmsg(regs->ebx, (const struct msghdr*)regs->ecx, regs->edx));
}

ssize_t	sys_recvmsg(registers_t *regs)
{
	return (recvmsg(regs->ebx, (struct msghdr*)regs->ecx, regs->edx));
}

static ssize_t	(*syscall[384])(registers_t*) =
{
	[SYS_exit] 		= sys_exit,
	[SYS_fork] 		= sys_fork,
	[SYS_read] 		= sys_read,
	[SYS_write] 		= sys_write,
	[SYS_open] 		= sys_open,
	[SYS_close] 		= sys_close,
	[SYS_waitpid]		= sys_waitpid,
	[SYS_wait] 		= sys_wait,
	[SYS_execve] 		= sys_execve,
	[SYS_chdir]		= sys_chdir,
	[SYS_getpid]		= sys_getpid,
	[SYS_setuid]		= sys_setuid,
	[SYS_getuid] 		= sys_getuid,
	[SYS_access] 		= sys_access,
	[SYS_kill] 		= sys_kill,
	[SYS_dup] 		= sys_dup,
	[SYS_pipe] 		= sys_pipe,
	[SYS_brk]		= sys_brk,
	[SYS_setgid]		= sys_setgid,
	[SYS_getgid]		= sys_getgid,
	[SYS_signal] 		= sys_signal,
	[SYS_dup2]		= sys_dup2,
	[SYS_symlink]		= sys_symlink,
	[SYS_readlink]		= sys_readlink,
	[SYS_mmap] 		= sys_mmap,
	[SYS_munmap]		= sys_munmap,
	[SYS_stat]		= sys_stat,
	[SYS_lstat]		= sys_lstat,
	[SYS_fstat]		= sys_fstat,
	[SYS_init_module]	= sys_init_module,
	[SYS_del_module]	= sys_del_module,
	[SYS_getdents]		= sys_getdents,
	[SYS_getcwd] 		= sys_getcwd,
	[SYS_socketpair]	= sys_socketpair,
	[SYS_sendmsg]		= sys_sendmsg,
	[SYS_recvmsg]		= sys_recvmsg,
};

void 	syscall_callback(registers_t *regs)
{
	kprintf("Syscall %d requested!\n", regs->eax);

	// check if regs->eax (syscall number) exists
	if (regs->eax < 384 && syscall[regs->eax] != NULL)
		regs->eax = syscall[regs->eax](regs);
	else
		kdebug("Error: Syscall not implemented.\n");
}

