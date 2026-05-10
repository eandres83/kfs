#include "task.h"

ssize_t mi_write(char *str, size_t len)
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (4), "b" (1), "c" (str), "d" (len) : "memory");
	return (ret);
}

void mi_exit(uint32_t status)
{
	asm volatile("int $0x80" : : "a" (1), "b" (status) : "memory");
	while (1);
}

ssize_t mi_fork()
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (2) : "memory");
	return (ret);
}

int32_t mi_wait(uint32_t *status)
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (7), "b" (status) : "memory");
	return (ret);
}

int32_t mi_kill(uint32_t pid, uint32_t sig)
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (37), "b" (pid), "c" (sig) : "memory");
	return (ret);
}

int32_t mi_execve(char *file_path, char **argv, char **envp)
{
	int32_t	ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (11), "b" (file_path), "c" (argv), "d" (envp) : "memory");
	return (ret);
}

int32_t mi_signal(uint32_t num, void (*function)())
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (48), "b" (num), "c" (function) : "memory");
	return (ret);
}

//void	mi_manejador_signal()
//{
//	mi_write("A", 1);
//	mi_exit(99);
//}
//
//void proceso_test_syscall()
//{
//	uint32_t status;
//
//	int32_t res = mi_signal(999, mi_manejador_signal);
//	if (res == -1)
//		mi_write("E", 1);
//	int32_t res2 = mi_signal(15, mi_manejador_signal);
//	if (res2 == -1)
//		mi_write("E", 1);
//	int32_t pid = mi_fork();
//	if (pid == 0)
//	{
//		while (1);
//	}
//	else if (pid > 0)
//	{
//		int32_t res3 = mi_kill(pid, 15);
//		if (res3 == -1)
//			mi_write("E", 1);
//
//		mi_wait(&status);
//		while (1);
//	}
//	mi_exit(-1);
//}
//void proceso_test_syscall()
//{
//	int32_t pid;
//	uint32_t status = 0;
//
//	pid = mi_fork();
//
//	// el hijo
//	if (pid == 0)
//	{
//		char *file_path = "/home/kfs/a.out";
//		char **argv = (char **)kmalloc(sizeof(char*) * 2);
//		argv[0] = "/bin/ls";
//		argv[1] = "-la";
//		argv[2] = NULL;
//		char **envp = (char **)kmalloc(sizeof(char*) * 1);
//		envp[0] = (char*)kmalloc(sizeof(char) * 1);
//		envp[0][0] = 'a';
//		mi_execve(file_path, argv, envp);
//
//		mi_exit(0);
//	}
//	// el padre
//	else if (pid > 0)
//	{
//		mi_wait(&status);
//		while(1);
//	}
//}
//__attribute__((section(".user_text"))) void test_signal()
//{
//	mi_write("B", 1);
//	mi_exit(0);
//}
//__attribute__((section(".user_text"))) void proceso_test_syscall()
//{
//	uint32_t status = 0;
//	int32_t pid;
//
//	mi_write(msg_fork, sizeof(msg_fork) -1);
//	pid = mi_fork();
//	// el hijo
//	if (pid == 0)
//	{
//		mi_write(msg_hijo1, sizeof(msg_hijo1) - 1);
//		mi_exit(42);
//	}
//	// el padre
//	else if (pid > 0)
//	{
//		mi_wait(&status);
//		if (status == 42)
//			mi_write(msg_wait, sizeof(msg_wait) - 1);
//
//		pid = mi_fork();
//		if (pid == 0)
//		{
//			mi_signal(9, mi_manejador_signal);
//			mi_write(msg_hijo2, sizeof(msg_hijo2) - 1);
//			while (1)
//				for (volatile int i = 0; i < 500000; i++);
//		}
//		else
//		{
//			for (volatile int i = 0; i < 1000000; i++);
//	
//			mi_write(msg_kill, sizeof(msg_kill) -1);
//			mi_kill(pid, 9);
//			mi_wait(&status);
//			if (status == 99)
//				mi_write(msg_fin, sizeof(msg_fin) - 1);
//		}
//	}
//	while(1)
//	{
//		mi_write(".", 1);
//		for (volatile int i = 0; i < 50000000; i++);
//	}
//}
//
