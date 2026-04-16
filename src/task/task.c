#include "task.h"

// temporal solo para testear procesos
static int32_t mi_user_write_syscall(char *str, size_t len)
{
	int32_t ret;

	asm volatile("int $0x80" : "=a" (ret) : "a" (4), "b" (1), "c" (str), "d" (len) : "memory");

	return (ret);
}

static void proceso_A()
{
	char *letra = "A";
	while (1)
	{
		mi_user_write_syscall(letra, 1);
		for (volatile int i = 0; i < 1000000; i++);
	}
}

static void proceso_B()
{
	char *letra = "B";
	while (1)
	{
		mi_user_write_syscall(letra, 1);
		for (volatile int i = 0; i < 1000000; i++);
	}
}
// ---------------------------------------------------------

static uint32_t next_pid = 1;
struct context *scheduler_context;
struct tss_entry *tss;
proc_t process[64];
proc_t *current_process;

static proc_t *find_process()
{
	for (int i = 0; i < 64; i++)
	{
		if (process[i].state == UNUSED)
		{
			process[i].state = EMBRYO;
			process[i].id = i;
			process[i].uid = 0; // root process
			process[i].pid = next_pid++;
			return (&process[i]);
		}
	}
	return (NULL);
}

void start_user_process()
{
	void *phys_stack = pmm_map_page();
	void *virt_stack = (void *)(0xBFFFF000);
	vmm_map_page(phys_stack, virt_stack, true);
	if (!virt_stack)
		return ;
	char	*top_stack = (char*)virt_stack + 4096;
	uint32_t entry_point = (uint32_t)(size_t)current_process->user_eip;

	jump_to_usermode(entry_point, (uint32_t)top_stack);
}

void create_process(void (*function)())
{
	proc_t *proc = find_process();
	if (proc == NULL)
		return ;
	// create kernel stack for a process
	proc->kstack = (char*)kmalloc(4096);
	if (!proc->kstack)
		return ;

	// request page directory
	create_memory_process(proc);

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
	// create user stack
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
				vmm_load_process_directory(process[i].pd);
				swtch(&scheduler_context, process[i].context);
			}
		}
	}
}

void iniciar_multitarea()
{
	kmemset(process, 0, sizeof(process));

	create_process(proceso_A);
	create_process(proceso_B);

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

// syscall for process

ssize_t fork(registers_t *regs)
{
	for (int i = 0; i < 64; i++)
	{
		if (process[i].state ==  UNUSED)
		{
			process[i].parent = current_process;
			process[i].pid = next_pid++;
			process[i].kstack = kmalloc(4096);
			if (!process[i].kstack)
				return (-1);
			kmemcpy(process[i].kstack, current_process->kstack, 4096);
			uint32_t offset_context = (uint32_t)current_process->context - (uint32_t)current_process->kstack;
			process[i].context = (struct context*)(process[i].kstack + offset_context);
			// copy pd parent to child process
			copy_parent_memory(&process[i]);

			// calcular offset para saber donde esta la estructura
			uint32_t offset_regs = (uint32_t)regs - (uint32_t)current_process->kstack;
			registers_t *hijo_regs = (registers_t*)(process[i].kstack + offset_regs);
			hijo_regs->eax = 0;

			process[i].state = RUNNABLE;
			return (process[i].pid);
		}
	}
	return (-1);
}

void exit_process(uint32_t status)
{
	current_process->state = ZOMBIE;
	current_process->exit_status = status;
	if (current_process->parent && current_process->parent->state == SLEEPING)
		current_process->parent->state = RUNNABLE;
	scheduler();
}

ssize_t wait(uint32_t *status)
{
	while (1)
	{
		bool child = false;
		uint32_t tmp_pid;
	
		for (int i = 0; i < 64; i++)
		{
			if (process[i].parent == current_process)
			{
				child = true;
				if (process[i].state == ZOMBIE)
				{
					*status = process[i].exit_status;
					tmp_pid = process[i].pid;
					kfree(process[i].kstack);
					pmm_free_page(process[i].pd);
					kmemset(&process[i], 0, sizeof(proc_t));
					return (tmp_pid);
				}
			}
		}
		if (child == false)
			return (-1);
		current_process->state = SLEEPING;
		yield();
	}
}

ssize_t signal(uint32_t signum, void (*function))
{
	if (signum >= 32)
		return (-1);
	current_process->signal_handlers[signum] = function;
	return (0);
}

ssize_t kill(uint32_t pid, uint32_t signal)
{
	if (pid == 0)
		return (-1);
	for (int i = 0; i < 64; i++)
	{
		if (process[i].pid == pid)
		{
			process[i].signals |= (1 << signal);
			return (0);
		}
	}
	return (-1);
}

ssize_t getuid()
{
	return (current_process->uid);
}

void find_signal(registers_t *regs)
{
	if (current_process->signals != 0)
	{
		for (int i = 0; i < 31; i++)
		{
			if (current_process->signals & (1 << i))
			{
				current_process->signals &= ~(1 << i);
				void *handler = (void*)current_process->signal_handlers[i];
				if (handler == NULL)
				{
					if (i == 9)
						exit_process(0);
				}
				else
				{
					regs->useresp -= 4;
					uint32_t *pila_user = (uint32_t*)regs->useresp;
					*pila_user = regs->eip;
					regs->eip = (uint32_t)handler;
					break;
				}
			}
		}
	}
}

