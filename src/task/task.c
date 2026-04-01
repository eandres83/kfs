#include "task.h"

struct tss_entry *tss;

int32_t mi_user_write_syscall(char *str, size_t len)
{
	int32_t ret;

	asm volatile("int $0x80" : "=a" (ret) : "a" (4), "b" (1), "c" (str), "d" (len) : "memory");

	return (ret);
}

void	mi_funcion()
{
	char *mensaje = "Hola desde el ring 3 via syscall\n";

	mi_user_write_syscall(mensaje, kstrlen(mensaje));

	while (1);
}

void test_ring3()
{
	void *user_stack = kmalloc(4096);

	uint32_t stack_top = (uint32_t)user_stack + 4096;

	kprintf("Saltando al ring 3\n");
	jump_to_usermode((uint32_t)mi_funcion, stack_top);
}

