#include "modules.h"
#include "task/task.h"
#include "task/elf.h"
#include "arch/i386/lib/uaccess.h"
#include "modules/events.h"

struct modules modules[MAX_MODULES];

static uint32_t module_bitmap[32] = {0};

static inline void bitmap_set_bit(size_t indx)
{
	module_bitmap[indx / 32] |= (1 << (indx % 32));
}

static inline void bitmap_unset_bit(size_t indx)
{
	module_bitmap[indx / 32] &= ~(1 << (indx % 32));
}

static bool check_if_root()
{
	proc_t *process = get_current_process();
	if (process->euid != 0)
		return (false);
	return (true);
}

void	module_free(void *addr, size_t nb)
{
	uint32_t bitmap_idx = ((uint32_t)addr - MODULE_VADDR_START) / 4096;

	for (size_t i = 0; i < nb; i++, bitmap_idx++)
	{
		bitmap_unset_bit(bitmap_idx);
		vmm_unmap_page(addr + (i * 4096));
	}
}

void	*module_alloc(size_t nb)
{
	size_t cont = 0;
	size_t j = 0;
	size_t i = 0;

	if (nb == 0)
		return (NULL);
	for (i = 0; i < 32; i++)
	{
		for (j = 0; j < 32; j++)
		{
			if ((module_bitmap[i] & (1 << j)) == 0)
			{
				cont++;
				if (cont == nb)
					goto calculate_addr;
			}
			else
				cont = 0;
		}
	}
	if (cont != nb)
		return (NULL);
calculate_addr:
	uint32_t bitmap_idx = (i * 32) + j;
	uint32_t final_idx = bitmap_idx - (nb - 1);
	void	*virt_addr = (void*)MODULE_VADDR_START + (final_idx * 4096);

	for (i = 0; i < nb; i++)
		bitmap_set_bit(final_idx + i);
	set_attributes(virt_addr, nb);
	return (virt_addr);
}

void	remove_permision(char *content, struct elf32_ehdr *elf, uint32_t *tmp_array)
{
	struct elf32_shdr *section = (struct elf32_shdr*)((char*)content + elf->e_shoff);
	for (int i = 0; i < elf->e_shnum; i++)
	{
		if ((section->sh_type == SHT_PROGBITS) && (section->sh_flags & (SHF_ALLOC | SHF_EXECINSTR)) && (section->sh_size > 0))
		{
			void *addr = (void*)tmp_array[i];
			if (!(section->sh_flags & SHF_WRITE))
				remove_attribute(addr, ALIGN_PAGE(section->sh_size));
		}
		section = (struct elf32_shdr*)((char*)section + elf->e_shentsize);
	}
}

ssize_t	count_and_alloc_module(char *content, struct elf32_ehdr *elf, struct modules *module)
{
	struct elf32_shdr *section = (struct elf32_shdr*)((char*)content + elf->e_shoff);
	module->nb_page = 0;
	for (int i = 0; i < elf->e_shnum; i++)
	{
		if (((section->sh_type == SHT_NOBITS) || (section->sh_type == SHT_PROGBITS)) && (section->sh_flags & SHF_ALLOC) && (section->sh_size > 0))
		{
			module->nb_page += ALIGN_PAGE(section->sh_size);
		}
		section = (struct elf32_shdr*)((char*)section + elf->e_shentsize);
	}
	if (module->nb_page == 0)
		return (-1);

	module->base_address = module_alloc(module->nb_page);
	if (!module->base_address)
		return (-1);
	kdebug("el total de paginas que pido -> %d\n", module->nb_page);
	return (0);
}

ssize_t	alloc_sections(char *content, struct elf32_ehdr *elf, uint32_t *tmp_array, struct modules *module)
{
	if (count_and_alloc_module(content, elf, module) == -1)
		return (-1);
	char *current_dest = (char*)module->base_address;
	struct elf32_shdr *section = (struct elf32_shdr*)((char*)content + elf->e_shoff);
	for (int i = 0; i < elf->e_shnum; i++)
	{
		char *real_offset = (char*)content + section->sh_offset;
		if (((section->sh_type == SHT_NOBITS) || (section->sh_type == SHT_PROGBITS)) && (section->sh_flags & SHF_ALLOC) && (section->sh_size > 0))
		{
			tmp_array[i] = (uint32_t)current_dest;
			if (section->sh_type == SHT_NOBITS)
				kmemset(current_dest, 0, section->sh_size);
			else
				kmemcpy(current_dest, real_offset, section->sh_size);
			current_dest += (ALIGN_PAGE(section->sh_size) * 4096);
		}
		section = (struct elf32_shdr*)((char*)section + elf->e_shentsize);
	}
	return (0);
}

ssize_t	realocation_symbols(char *content, struct elf32_ehdr *elf, uint32_t *tmp_array)
{
	struct elf32_shdr *section = (struct elf32_shdr*)((char*)content + elf->e_shoff);
	struct elf32_shdr *section_base = section;
	for (int i = 0; i < elf->e_shnum; i++)
	{
		if (section->sh_type == SHT_REL && tmp_array[section->sh_info] != 0)
		{
			struct elf32_shdr *tmp_section = &section_base[section->sh_link];
			struct elf32_shdr *sect = &section_base[tmp_section->sh_link];
			struct elf32_sym  *symbol = (struct elf32_sym*)((char*)content + tmp_section->sh_offset);
			struct elf32_rel  *rel = (struct elf32_rel*)((char *)content + section->sh_offset);
			size_t	size = section->sh_size / section->sh_entsize;
			for (size_t j = 0; j < size; j++)
			{
				struct elf32_rel *tmp_rel = &rel[j];
				struct elf32_sym *tmp_sym = &symbol[ELF32_R_SYM(tmp_rel->r_info)];
				char *name = (content + sect->sh_offset) + tmp_sym->st_name;
				uint32_t S = 0;
				if (tmp_sym->st_shndx == 0)
				{
					for (size_t x = 0; x < size_symbols; x++)
					{
						if (kstrcmp(array_symbols[x].name, name) == 0)
						{
							struct symbol_table *table = &array_symbols[x];
							kdebug("dentro del puto kstrcmp -> %x\n", array_symbols[x].addr);
							kdebug("dentro del puto kstrcmp -> %s\n", array_symbols[x].name);
							S = table->addr; 
							break;
						}
					}
				}
				else if (tmp_sym->st_shndx >= 0xFF00)
					S = tmp_sym->st_value;
				else
					S = tmp_array[tmp_sym->st_shndx] + tmp_sym->st_value;
				uint32_t P = tmp_array[section->sh_info] + tmp_rel->r_offset;
				uint32_t A = *(uint32_t*)P;
				if (ELF32_R_TYPE(tmp_rel->r_info) == R_386_32)
					*(uint32_t*)P = S + A;
				else if (ELF32_R_TYPE(tmp_rel->r_info) == R_386_PC32)
					*(uint32_t*)P = S + A - P;
			}
		}
		section = (struct elf32_shdr*)((char*)section + elf->e_shentsize);
	}
	return (0);
}

ssize_t	search_init_function(char *content, struct elf32_ehdr *elf, uint32_t *tmp_array, struct modules *module)
{
	struct elf32_shdr *section = (struct elf32_shdr*)((char*)content + elf->e_shoff);
	struct elf32_shdr *section_base = section;
	for (int i = 0; i < elf->e_shnum; i++)
	{
		if (section->sh_type == SHT_SYMTAB)
		{
			struct elf32_shdr *tmp_section = &section_base[section->sh_link];
			struct elf32_sym  *symbol = (struct elf32_sym*)((char*)content + section->sh_offset);
			char *strtab_base = (char*)content + tmp_section->sh_offset;
			size_t	size = section->sh_size / section->sh_entsize;
			for (size_t j = 0; j < size; j++)
			{
				struct elf32_sym *tmp_sym = &symbol[j];
				char *name = strtab_base + tmp_sym->st_name;
				if (kstrcmp(name, "init_module") == 0)
					module->init = (int (*)())(tmp_array[tmp_sym->st_shndx] + tmp_sym->st_value);
				else if (kstrcmp(name, "cleanup_module") == 0)
					module->cleanup = (void (*)())(tmp_array[tmp_sym->st_shndx] + tmp_sym->st_value);
			}
		}
		section = (struct elf32_shdr*)((char*)section + elf->e_shentsize);
	}
	return (0);
}

ssize_t	insmod(char *binary)
{
	if (!check_if_root())
		return (-1);
	char *kernel_addr = kmalloc(256);
	if (!kernel_addr)
		return (-1);
	copy_from_user_wrap((void*)kernel_addr, (void*)binary, 256);
	kernel_addr[255] = '\0';

	struct modules *module = NULL;
	asm volatile ("cli");
	for (int i = 0; i < MAX_MODULES; i++)
	{
		if (modules[i].state == MOD_STATE_EMPTY)
		{
			modules[i].state = MOD_STATE_LOADING;
			module = &modules[i];
			break;
		}
	}
	asm volatile ("sti");
	if (module == NULL)
		return (-1);
	kmemset(module, 0, sizeof(struct modules));
	struct vfs_node *node = get_vfs_node_path(kernel_addr);
	if (node == 0x0)
		return (kfree(kernel_addr), -1);
	kfree(kernel_addr);
	kstrlcpy(module->name, node->name, 255);
	char *content = (char*)kmalloc(node->size);
	if (!content)
		return (-1);
	ssize_t res = node->ops->read(node, content, node->size, 0);
	if (res == -1)
		return (kfree(content), -1);
	struct elf32_ehdr *elf = (struct elf32_ehdr*)content;
	if (check_binary(elf) == false)
		return (kfree(content), -1);

	uint32_t *tmp_array = kmalloc(sizeof(uint32_t) * elf->e_shnum);
	if (!tmp_array)
		return (kfree(content), -1);
	kmemset(tmp_array, 0, (sizeof(uint32_t) * elf->e_shnum));

	if (alloc_sections(content, elf, tmp_array, module) == -1)
		return (kfree(content), kfree(tmp_array), -1);

	if (realocation_symbols(content, elf, tmp_array) == -1)
		return (kfree(content), kfree(tmp_array), module_free(module->base_address, module->nb_page), -1);

	if (search_init_function(content, elf, tmp_array, module) == -1)
		return (kfree(content), kfree(tmp_array), module_free(module->base_address, module->nb_page), -1);

	if (module->init == NULL || module->cleanup == NULL)
		return (kfree(content), kfree(tmp_array), module_free(module->base_address, module->nb_page), -1);
	remove_permision(content, elf, tmp_array);

	kdebug("el puto base_addr -> %x\n", module->base_address);
	int retur = module->init();
	if (retur != 0)
	{
		kprintf("El result -> %d\n", retur);
		return (kfree(content), kfree(tmp_array), module_free(module->base_address, module->nb_page), -1);
	}
	module->state = MOD_STATE_LIVE;
	kfree(content);
	kfree(tmp_array);
	return (0);
}

ssize_t	rmmod(char *module_name)
{
	if (!check_if_root())
		return (-1);
	char *kernel_addr = kmalloc(256);
	if (!kernel_addr)
		return (-1);
	copy_from_user_wrap((void*)kernel_addr, (void*)module_name, 256);
	kernel_addr[255] = '\0';

	struct modules *module = NULL;
	asm volatile ("cli");
	for (int i = 0; i < MAX_MODULES; i++)
	{
		kdebug("el name -> %s, el module_name -> %s\n", kernel_addr, modules[i].name);
		if ((kstrcmp(modules[i].name, kernel_addr) == 0) && (modules[i].state == MOD_STATE_LIVE))
		{
			module = &modules[i];
			module->state = MOD_STATE_GOING;
			break;
		}
	}
	asm volatile ("sti");
	if (module == NULL)
		return (kprintf("Module name not found :(\n"), kfree(kernel_addr), -1);

	if (module->cleanup == NULL)
		return (kfree(kernel_addr), -1);
	module->cleanup();
	desregister_callback(kernel_addr);
	module_free(module->base_address, module->nb_page);

	kmemset(module, 0, sizeof(struct modules));
	module->state = MOD_STATE_EMPTY;
	kfree(kernel_addr);
	return (0);
}

void	listmod()
{
	int count = 0;
	for (int i = 0; i < MAX_MODULES; i++)
	{
		if (modules[i].state == MOD_STATE_LIVE)
		{
			kprintf("%s    ", modules[i].name);
			kprintf("%d\n", (modules[i].nb_page * 4096));
			count = 1;
		}
	}
	if (count == 0)
		kprintf("There are no modules active\n");
}

