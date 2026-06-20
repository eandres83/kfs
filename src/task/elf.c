#include "task/elf.h"
#include "task/task.h"

bool	check_binary(struct elf32_ehdr *elf)
{
	if (elf->e_ident[EI_MAG0] != 0x7f || elf->e_ident[EI_MAG1] != 'E' || elf->e_ident[EI_MAG2] != 'L' || elf->e_ident[EI_MAG3] != 'F')
		return (kdebug("Error: not a elf file :(\n"), false);
	if (elf->e_ident[EI_CLASS] != 1) // 
		return (kdebug("Error: not a 32 bits file\n"), false);
	if (elf->e_type != 2 && elf->e_type != 1) // check file type
		return (kdebug("Error: not a executable file\n"), false);
	if (elf->e_machine != 3) // check intel architecture
		return (kdebug("Error: not x86/i386 architecture\n"), false);
	return (true);
}

static char **malloc_var(char **str)
{
	uint32_t len = 0;
	while (str[len] != NULL)
		len++;
	char **res = (char **)kmalloc(sizeof(char*) * (len + 1));
	if (!res)
		return (NULL);
	for (int i = 0; i < (int)len; i++)
	{
		res[i] = (char*)kmalloc((sizeof(char) * kstrlen(str[i])) + 1);
		if (!res[i])
			return (NULL);
		kstrcpy(res[i], str[i]);
	}
	res[len] = NULL;
	return (res);
}

static char *make_stack(char **envp, char **argv)
{
	void *phys = pmm_map_page();
	void *virt = (void*)0xBFFFF000;
	vmm_map_page(phys, virt, true);
	char *user_stack = (char*)virt + 4096;

	uint32_t addr_save_envp[64];
	uint32_t env_len;
	for (env_len = 0; envp[env_len] != NULL; env_len++)
	{
		size_t len = kstrlen(envp[env_len]) + 1;
		user_stack = user_stack - len;
		kstrcpy(user_stack, envp[env_len]);
		addr_save_envp[env_len] = (uint32_t)user_stack;
	}
	uint32_t align = (uint32_t)user_stack % 4;
	if (align != 0)
		user_stack -= align;

	int32_t argc = 0;
	while (argv[argc] != NULL)
		argc++;
	uint32_t addr_save_argv[64];
	int32_t tmp_argc = argc - 1;
	while (tmp_argc >= 0)
	{
		size_t len_last_argc = kstrlen(argv[tmp_argc]) + 1;
		user_stack = user_stack - len_last_argc;
		kstrcpy(user_stack, argv[tmp_argc]);
		addr_save_argv[tmp_argc] = (uint32_t)user_stack;
		tmp_argc--;
	}
	align = (uint32_t)user_stack % 4;
	if (align != 0)
		user_stack -= align;

	user_stack -= 4;
	*(uint32_t*)user_stack = 0; // NULL de envp
	// aqui hacer bucle para meter env
	for (int i = env_len - 1; i >= 0; i--)
	{
		user_stack -= 4;
		*(uint32_t*)user_stack = addr_save_envp[i];
	}
	user_stack -= 4;
	*(uint32_t*)user_stack = 0; // NULl de argv
	// hacer un bucle recorriendo argv y metiendolo en user_stack
	for (int i = argc - 1; i >= 0; i--)
	{
		user_stack -= 4;
		*(uint32_t*)user_stack = addr_save_argv[i];
	}
	// por ultimo meter argc y done
	user_stack -= 4;
	*(uint32_t*)user_stack = (uint32_t)argc;
	return (user_stack);
}

ssize_t	execve(char *file_path, char **user_argv, char **user_envp, registers_t *regs)
{
	if (file_path == NULL || user_argv == NULL || user_envp == NULL)
		return (-1);
	// leer el binario y comprobar que es elf valido
	struct vfs_node *node = get_vfs_node_path(file_path);
	if (node == 0x0)
		return (-1);
	char *content = (char*)kmalloc(node->size);
	if (!content)
		return (-1);
	ssize_t res = node->ops->read(node, content, node->size, 0);
	if (res == -1)
		return (-1);
	struct elf32_ehdr *elf = (struct elf32_ehdr*)content;
	if (check_binary(elf) == false)
		return (kfree(content), -1);

	// guardar los argumentos seguros antes de cambiar el pd
	char **argv = malloc_var(user_argv);
	if (!argv)
		return (kfree(content), -1);
	char **envp = malloc_var(user_envp);
	if (!envp)
		return (double_free(argv), kfree(content), -1);

	// cambiar al nuevo pd y destruir el antiguo
	proc_t	*current_process = get_current_process();
	void *old_pd = current_process->pd;
	current_process->mmap_count = 0;

	for (int i = 0; i < 32; i++)
		current_process->mmap_allocation[i] = 0;
	for (int i = 0; i < 32; i++)
		current_process->signal_handlers[i] = 0;

	create_memory_process(current_process);
	vmm_load_process_directory(current_process->pd);

	struct elf32_phdr *program = (struct elf32_phdr*)((char*)content + elf->e_phoff);
	for (int i = 0; i < elf->e_phnum; i++)
	{
		if (program->p_type == PT_LOAD)
		{
			if (program->p_memsz == 0 || program->p_vaddr < 0x1000)
			{
				program = (struct elf32_phdr*)((char*)program + elf->e_phentsize);
				continue;
			}
			uint32_t start_page = program->p_vaddr & 0xFFFFF000;
			uint32_t end_vaddr = program->p_vaddr + program->p_memsz;
			uint32_t end_page = (end_vaddr + 4095) & 0xFFFFF000;
			uint32_t nb_page = (end_page - start_page) / 4096;
			for (uint32_t j = 0; j < nb_page; j++)
			{
				void *virt_addr = (void*)(start_page + (j * 4096));
				if (check_user_addr((uint32_t)virt_addr) == false)
				{
					void *phys = pmm_map_page();
					if (!phys)
						return (double_free(argv), double_free(envp), kfree(content), -1);
					vmm_map_page(phys, virt_addr, true);
					kmemset(virt_addr, 0, 4096);
				}
			}
			if (program->p_filesz > 0)
				kmemcpy((void*)program->p_vaddr, (void*)((char*)content + program->p_offset), program->p_filesz);
			if (program->p_memsz > program->p_filesz)
				kmemset((void*)(program->p_vaddr + program->p_filesz), 0, program->p_memsz - program->p_filesz);
		}
		program = (struct elf32_phdr*)((char*)program + elf->e_phentsize);
	}
	char *user_stack = make_stack(envp, argv);
	regs->useresp = (uint32_t)user_stack;
	regs->eip = elf->e_entry;

	if (old_pd != NULL)
		free_page_directory(old_pd);
	return (double_free(argv), double_free(envp), kfree(content), 0);
}

