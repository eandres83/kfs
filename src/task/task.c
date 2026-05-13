#include "task.h"

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

static void start_user_process()
{
	void *phys_stack = pmm_map_page();
	void *virt_stack = (void *)(0xBFFFF000);
	vmm_map_page(phys_stack, virt_stack, true);
	if (!virt_stack)
		return ;
	current_process->user_stack = (char*)virt_stack + 4096;
	uint32_t entry_point = (uint32_t)(size_t)current_process->user_eip;

	jump_to_usermode(entry_point, (uint32_t)current_process->user_stack);
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

	kmemset(proc->pwd, 0, 256);
	kstrcpy(proc->pwd, "/");

	proc->node = vfs;

	proc->mmap_count = 0;
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
				asm volatile ("cli");
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

static void enter_user_mode()
{
	uint32_t entry_point = (uint32_t)(size_t)current_process->user_eip;
	uint32_t stack = (uint32_t)current_process->user_stack;

	jump_to_usermode(entry_point, stack);
}

void create_init_process()
{
	proc_t *new_proc = find_process();
	new_proc->kstack = (char*)kmalloc(4096);
	if (!new_proc->kstack)
		return ;
	current_process = new_proc;

	char *argv[] = {"/home/kfs/test_bin", NULL};
	char *envp[] = {"", NULL};
	registers_t regs;
	int32_t res = execve("/home/kfs/test_bin", argv, envp, &regs);
	if (res == -1)
	{
		kprintf("Fatal error when init_main_process execve, soo bad :(\n");
		return ;
	}
	char *stack_top = new_proc->kstack + 4096;
	new_proc->context = (struct context *)stack_top - 1;
	new_proc->context->edi = 0;
	new_proc->context->esi = 0;
	new_proc->context->ebx = 0;
	new_proc->context->ebp = 0;

	new_proc->context->eip = (uint32_t)enter_user_mode;
	new_proc->user_eip = (char*)regs.eip;
	new_proc->user_stack = (char*)regs.useresp;

	new_proc->state = RUNNABLE;
	scheduler();
}

void yield()
{
	if (current_process->state == RUNNING)
		current_process->state = RUNNABLE;

	swtch(&current_process->context, scheduler_context);
}

void kill_process(char *motivo)
{
	
	kprintf("\n[Kernel] Porceso PID %d asesinado por exception: %s \n",
		current_process->pid, motivo);
	kill(current_process->pid, 9);
	exit_process(139);
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
			// copy pd parent to child process
			copy_parent_memory(&process[i]);

			// calcular offset para saber donde esta la estructura
			uint32_t offset_regs = (uint32_t)regs - (uint32_t)current_process->kstack;
			registers_t *hijo_regs = (registers_t*)(process[i].kstack + offset_regs);
			hijo_regs->eax = 0;

			uint32_t *reg_ptr = (uint32_t*)hijo_regs -1;
			*reg_ptr = (uint32_t)hijo_regs;
			process[i].context = (struct context*)reg_ptr - 1;

			process[i].context->edi = 0;
			process[i].context->esi = 0;
			process[i].context->ebx = 0;
			process[i].context->ebp = 0;
			process[i].context->eip = (uint32_t)fork_child_exit;

			process[i].user_stack = (char*)hijo_regs;

			for (int s = 0; s < 32; s++)
				process[i].signal_handlers[s] = current_process->signal_handlers[s];

			for (int a = 0; a < 32; a++)
				process[i].mmap_allocation[a] = current_process->mmap_allocation[a];
			process[i].mmap_count = current_process->mmap_count;
			process[i].state = RUNNABLE;
			process[i].node = current_process->node;
			kstrcpy(process[i].pwd, current_process->pwd);

			for (int j = 0; j < 63; j++)
			{
				if (current_process->fd_table[j] != NULL)
				{
					process[i].fd_table[j] = current_process->fd_table[j];
					process[i].fd_table[j]->ref_count++;
				}
			}
			return (process[i].pid);
		}
	}
	return (-1);
}

ssize_t mmap(ssize_t size)
{
	if (size < 0)
		return (-1);
	int index = -1;
	for (int i = 0; i < 32; i++)
	{
		if (current_process->mmap_allocation[i] == 0)
		{
			index = i;
			break;
		}
	}
	if (index == -1)
		return (-1);

	uint32_t start_vaddr = 0x40000000 + (current_process->mmap_count * 4096);
	uint32_t virt_addr = start_vaddr;

	size_t	total_size = (size + 4095) / 4096;

	for (uint32_t i = 0; i < total_size; i++)
	{
		void *phys_addr = pmm_map_page();
		if (!phys_addr)
			return (-1);
		vmm_map_page(phys_addr, (void*)virt_addr, true);
		virt_addr += 4096;
	}

	current_process->mmap_allocation[index] = virt_addr;
	current_process->mmap_count++;

	return ((ssize_t)start_vaddr);
}

ssize_t munmap(void *addr, size_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{

		vmm_unmap_page(addr);
		for (int i = 0; i < 32; i++)
		{
			if (current_process->mmap_allocation[i] == (uint32_t)addr)
			{
				current_process->mmap_allocation[i] = 0;
				current_process->mmap_count--;
				break;
			}
		}
		addr = (void*)((uint32_t)addr + 4096);
	}
	return (0);
}

void exit_process(uint32_t status)
{
	current_process->state = ZOMBIE;
	current_process->exit_status = status;
	if (current_process->parent && current_process->parent->state == SLEEPING)
		current_process->parent->state = RUNNABLE;

	swtch(&current_process->context, scheduler_context);
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
					if (status != NULL)
						*status = process[i].exit_status;
					tmp_pid = process[i].pid;
					if (process[i].kstack)
						kfree(process[i].kstack);
					if (process[i].pd)
						pmm_free_page(process[i].pd);
					for (int a = 0; a < 32; a++)
					{
						if (process[i].mmap_allocation[a] != 0)
							munmap((void*)process[i].mmap_allocation[a], 4096);
					}
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
	current_process->signal_handlers[signum] = (uint32_t)function;
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

// helper function por fs
proc_t	*get_current_process()
{
	return (current_process);
}

void	set_current_process()
{
	current_process = kmalloc(sizeof(proc_t));
	current_process->node = vfs;
	kmemset(current_process->pwd, 0, 256);
	kstrcpy(current_process->pwd, "/");
}

struct vfs_node *get_current_node()
{
	return (current_process->node);
}

char *get_current_pwd()
{
	return (current_process->pwd);
}

void set_new_node(struct vfs_node *new_node)
{
	current_process->node = new_node;
}

void set_new_pwd(char *path)
{
	kmemset(current_process->pwd, 0, 256);
	kstrcpy(current_process->pwd, path);
}

