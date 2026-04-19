#include "task.h"

//__attribute__((section(".user_data"))) char letra_A[] = "A";
//__attribute__((section(".user_data"))) char letra_B[] = "B";
//
//// temporal solo para testear procesos
//__attribute__((section(".user_text"))) static int32_t mi_user_write_syscall(char *str, size_t len)
//{
//	int32_t ret;
//
//	asm volatile("int $0x80" : "=a" (ret) : "a" (4), "b" (1), "c" (str), "d" (len) : "memory");
//
//	return (ret);
//}
//
//__attribute__((section(".user_text"))) static void proceso_A()
//{
////	char *letra = "A";
//	while (1)
//	{
//		mi_user_write_syscall(letra_A, 1);
//		for (volatile int i = 0; i < 1000000; i++);
//	}
//}
//
//__attribute__((section(".user_text"))) static void proceso_B()
//{
////	char *letra = "B";
//	while (1)
//	{
//		mi_user_write_syscall(letra_B, 1);
//		for (volatile int i = 0; i < 1000000; i++);
//	}
//}
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

	proc->mmap_count = 0;
	// tmp
	proc->msg = kmalloc(sizeof(struct msg));
	if (!proc->msg)
		return ;
	proc->msg->max_buffer = 4096;
	proc->msg->write_index = 0;
	proc->msg->read_index = 0;
	proc->msg->buffer = kmalloc(4096);
	if (!proc->msg->buffer)
		return ;
	proc->text_start = 0;
	proc->text_end = 0;
	proc->data_start = 0;
	proc->data_end = 0;
	proc->bss_start = 0;
	proc->bss_end = 0;

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
				asm volatile ("sti");
			}
		}
	}
}

void iniciar_multitarea()
{
	kmemset(process, 0, sizeof(process));

//	create_process(proceso_A);
//	create_process(proceso_B);

	create_process(proceso_test_syscall);

	kprintf("Scheduler ahora\n");
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

			process[i].msg = kmalloc(sizeof(struct msg));
			if (!process[i].msg)
				return (-1);
			process[i].msg->max_buffer = 4096;
			process[i].msg->write_index = 0;
			process[i].msg->read_index = 0;
			process[i].msg->buffer = kmalloc(4096);
			if (!process[i].msg->buffer)
				return (-1);

			for (int s = 0; s < 32; s++)
				process[i].signal_handlers[s] = current_process->signal_handlers[s];

			for (int a = 0; a < 32; a++)
				process[i].mmap_allocation[a] = current_process->mmap_allocation[a];
			process[i].mmap_count = current_process->mmap_count;
			process[i].state = RUNNABLE;
			return (process[i].pid);
		}
	}
	return (-1);
}

static void	vmm_unmap(void *virt)
{
	page_directory *pd = (page_directory*)0xFFFFF000;

	// get current directory
	pd_entry *e = &pd->m_entries[PD_INDEX((uint32_t)virt)];
	if ((*e & PDE_PRESENT) != PDE_PRESENT)
		return ;

	// get table
	page_table *table = (page_table *)(0xFFC00000 + (PD_INDEX((uint32_t)virt) * PAGE_SIZE));

	// get page in table
	pt_entry *page = &table->m_entries[PT_INDEX((uint32_t)virt)];

	if (pt_entry_is_present(*page))
		kprintf("Page is present\n");
	else
		kprintf("Page is not present\n");

	// free page
//	vmm_free_page((void*)page);

//	reload_tlb(virt);
}

ssize_t test_mmap_present(void *addr)
{
	vmm_unmap(addr);
	return (0);
}

ssize_t mmap()
{
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

	uint32_t virt_addr = 0x40000000 + (current_process->mmap_count * 4096);

	void *phys_addr = pmm_map_page();
	if (!phys_addr)
		return (-1);
	vmm_map_page(phys_addr, (void*)virt_addr, true);

	current_process->mmap_allocation[index] = virt_addr;
	current_process->mmap_count++;

	return ((ssize_t)virt_addr);
}

ssize_t munmap(void *addr)
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
					if (process[i].msg != NULL && process[i].msg->buffer != NULL)
						kfree(process[i].msg->buffer);
					if (process[i].pd)
						pmm_free_page(process[i].pd);
					for (int a = 0; a < 32; a++)
					{
						if (process[i].mmap_allocation[a] != 0)
							munmap((void*)process[i].mmap_allocation[a]);
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

// temp solo para kfs-5
ssize_t sendmsg(uint32_t pid, char *msg, uint32_t len)
{
	for (int i = 0; i < 64; i ++)
	{
		if (process[i].pid == pid)
		{
			asm volatile ("cli");
			size_t byte_used = (process[i].msg->write_index - process[i].msg->read_index + 4096) % 4096;
			size_t free_size = 4095 - byte_used;
			if (free_size < len)
			{
				asm volatile ("sti");
				return (-1);
			}
			size_t space_until_end = 4096 - process[i].msg->write_index;
			if (len <= space_until_end)
				kmemcpy((char*)process[i].msg->buffer + process[i].msg->write_index, msg, len);
			else
			{
				kmemcpy((char*)process[i].msg->buffer + process[i].msg->write_index, msg, space_until_end);
				kmemcpy((char*)process[i].msg->buffer, msg + space_until_end, len - space_until_end);
			}
			process[i].msg->write_index = (process[i].msg->write_index + len) % 4096;
			asm volatile ("sti");
			return (len);
		}
	}
	return (-1);
}

ssize_t recvmsg(char *dest, size_t len)
{
	asm volatile ("cli");
	if (current_process->msg->read_index == current_process->msg->write_index)
		return (0);

	size_t ready_to_read = (current_process->msg->write_index - current_process->msg->read_index + 4096) % 4096;
	size_t min;
	if (ready_to_read > len)
		min = len;
	else
		min = ready_to_read;
	size_t space_until_end = 4096 - current_process->msg->read_index;

	if (min <= space_until_end)
		kmemcpy(dest, (char*)current_process->msg->buffer + current_process->msg->read_index, min);
	else
	{
		kmemcpy(dest, (char*)current_process->msg->buffer + current_process->msg->read_index, space_until_end);
		kmemcpy(dest + space_until_end, (char*)current_process->msg->read_index, min - space_until_end);
	}
	
	current_process->msg->read_index = (current_process->msg->read_index + min) % 4096;

	asm volatile ("sti");
	return (min);
}

