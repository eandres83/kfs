#include "task.h"

struct context *scheduler_context;
struct tss_entry *tss;
proc_t process[64];
proc_t *current_process;

int32_t mi_user_write_syscall(char *str, size_t len)
{
	int32_t ret;

	asm volatile("int $0x80" : "=a" (ret) : "a" (4), "b" (1), "c" (str), "d" (len) : "memory");

	return (ret);
}

void start_user_process();

void create_process(proc_t *proc, void (*function)())
{
	proc->kstack = (char*)kmalloc(4096);
	if (!proc->kstack)
		return ;
	// calcular el top stack
	char *stack_top = proc->kstack + 4096;
	// restar 1 struct context para que quede justo el tamano exacto
	proc->context = (struct context *)stack_top - 1;

	proc->context->edi = 0;
	proc->context->esi = 0;
	proc->context->ebx = 0;
	proc->context->ebp = 0;

	// donde tiene que saltar la CPU al hacer el ret
	proc->user_eip = (char*)function;
	proc->context->eip = (uint32_t)start_user_process;
	proc->state = RUNNABLE;
}

void scheduler()
{
	while (1)
	{
		for (int i = 0; i < 64; i++)
		{
			if (process[i].state == RUNNABLE)
			{
				char *top_stack = process[i].kstack + 4096;
				set_kernel_stack((uint32_t)top_stack);
				process[i].state = RUNNING;
				current_process = &process[i];
				swtch(&scheduler_context, process[i].context);
			}
		}
	}
}

void start_user_process()
{
	void *phys_stack = pmm_map_page();
	uint32_t offset = (current_process - process) * 4096;
	void *virt_stack = (void *)(0xBFFFF000 - offset);
	vmm_map_page(phys_stack, virt_stack, true);
	if (!virt_stack)
		return ;
	char		*top_stack = (char*)virt_stack + 4096;
	uint32_t	entry_point = (uint32_t)(size_t)current_process->user_eip;

	jump_to_usermode(entry_point, (uint32_t)top_stack);
}

void proceso_A()
{
	char *letra = "A";
	while (1)
	{
		mi_user_write_syscall(letra, 1);
		for (volatile int i = 0; i < 1000000; i++);
	}
}

void proceso_B()
{
	char *letra = "B";
	while (1)
	{
		mi_user_write_syscall(letra, 1);
		for (volatile int i = 0; i < 1000000; i++);
	}
}

void iniciar_multitarea()
{
	kmemset(process, 0, sizeof(process));

	create_process(&process[0], proceso_A);
	create_process(&process[1], proceso_B);

	kprintf("Scheduler ahora\n");
	scheduler();
}

void yield()
{
	for (int i = 0; i < 64; i++)
	{
		if (process[i].state == RUNNING)
		{
			process[i].state = RUNNABLE;
			swtch(&process[i].context, scheduler_context);
			return ;
		}
	}
}

