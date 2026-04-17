#include "task.h"

__attribute__((section(".user_data"))) char msg_fork[] = "Padre: Haciendo fork..\n";
__attribute__((section(".user_data"))) char msg_hijo1[] = "Hijo 1: Estoy vivo\n";
__attribute__((section(".user_data"))) char msg_wait[] = "Padre: Hijo 1 terminando wait..\n";
__attribute__((section(".user_data"))) char msg_hijo2[] = "Hijo 2: Estoy vivo\n";
__attribute__((section(".user_data"))) char msg_signal[] = "Hijo 2: senal recibida saliendo limpiamente\n";
__attribute__((section(".user_data"))) char msg_kill[] = "Padre: Matando al Hijo2 con senal 9\n";
__attribute__((section(".user_data"))) char msg_fin[] = "Padre: Todas las pruebas superadas\n";


__attribute__((section(".user_text"))) static int32_t mi_write(char *str, size_t len)
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (4), "b" (1), "c" (str), "d" (len) : "memory");
	return (ret);
}

__attribute__((section(".user_text"))) static void mi_exit(uint32_t status)
{
	asm volatile("int $0x80" : : "a" (1), "b" (status) : "memory");
	while (1);
}

__attribute__((section(".user_text"))) static int32_t mi_fork()
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (2) : "memory");
	return (ret);
}

__attribute__((section(".user_text"))) static int32_t mi_wait(uint32_t *status)
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (7), "b" (status) : "memory");
	return (ret);
}

__attribute__((section(".user_text"))) static int32_t mi_kill(uint32_t pid, uint32_t sig)
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (37), "b" (pid), "c" (sig) : "memory");
	return (ret);
}

__attribute__((section(".user_text"))) static int32_t mi_signal(uint32_t sig, void (*handler)())
{
	int32_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (48), "b" (sig), "c" (handler) : "memory");
	return (ret);
}

__attribute__((section(".user_text"))) static void mi_manejador_signal()
{
	mi_write(msg_signal, sizeof(msg_signal) - 1);
	mi_exit(99);
}

__attribute__((section(".user_text"))) void proceso_test_syscall()
{
	uint32_t status = 0;
	int32_t pid;

	mi_write(msg_fork, sizeof(msg_fork) -1);
	pid = mi_fork();
	// el hijo
	if (pid == 0)
	{
		mi_write(msg_hijo1, sizeof(msg_hijo1) - 1);
		mi_exit(42);
	}
	// el padre
	else if (pid > 0)
	{
			mi_wait(&status);
			if (status == 42)
				mi_write(msg_wait, sizeof(msg_wait) - 1);
		pid = mi_fork();
	
		if (pid == 0)
		{
			mi_signal(9, mi_manejador_signal);
			mi_write(msg_hijo2, sizeof(msg_hijo2) - 1);
			while (1)
				for (volatile int i = 0; i < 500000; i++);
		}
		else
		{
			for (volatile int i = 0; i < 10000000; i++);
	
			mi_write(msg_kill, sizeof(msg_kill) -1);
			mi_kill(pid, 9);
			mi_wait(&status);
			if (status == 99)
				mi_write(msg_fin, sizeof(msg_fin) - 1);
		}
	}
	while(1);
}

