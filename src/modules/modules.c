#include "modules.h"
#include "task/task.h"
#include "task/elf.h"

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

void	module_free(void *addr, size_t nb)
{
	uint32_t bitmap_idx = ((uint32_t)addr - MODULE_VADDR_START) / 4096;

	for (size_t i = 0; i < nb; i++, bitmap_idx++)
	{
		bitmap_unset_bit(bitmap_idx);
		vmm_unmap_page(addr + (i * 4096));
	}
}

void	*module_alloc(size_t nb, uint32_t flags)
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

	if (flags & SHF_WRITE)	
		set_attributes(virt_addr, nb, true);
	if (flags & SHF_EXECINSTR)
		set_attributes(virt_addr, nb, false);
	return (virt_addr);
}

ssize_t	alloc_sections(char *content, struct elf32_ehdr *elf, uint32_t *tmp_array)
{
	struct elf32_shdr *section = (struct elf32_shdr*)((char*)content + elf->e_shoff);
	for (int i = 0; i < elf->e_shnum; i++)
	{
		char *real_offset = (char*)content + section->sh_offset;
		if ((section->sh_type == SHT_NOBITS) && (section->sh_flags & (SHF_ALLOC | SHF_WRITE)))
		{
			void *addr = module_alloc(ALIGN(section->sh_size), section->sh_flags);
			if (!addr)
				return (-1);
			kmemset(addr, 0, section->sh_size);
			tmp_array[i] = (uint32_t)addr;
		}
		else if (section->sh_type == SHT_PROGBITS)
		{
			if (section->sh_flags & (SHF_ALLOC | SHF_WRITE))
			{
				void *addr = module_alloc(ALIGN(section->sh_size), section->sh_flags);
				if (!addr)
					return (-1);
				kmemcpy(addr, real_offset, section->sh_size);
				tmp_array[i] = (uint32_t)addr;
			}
			else if (section->sh_flags & (SHF_ALLOC | SHF_EXECINSTR))
			{
				void *addr = module_alloc(ALIGN(section->sh_size), section->sh_flags);
				if (!addr)
					return (-1);
				kmemcpy(addr, real_offset, section->sh_size);
				tmp_array[i] = (uint32_t)addr;
			}
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
		if (section->sh_type == SHT_REL)
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
							S = table->addr; 
						}
					}
				}
				else
					S = tmp_array[tmp_sym->st_shndx] + tmp_sym->st_value;
				uint32_t P = tmp_array[section->sh_info] + tmp_rel->r_offset;
				uint32_t A = *(uint32_t*)P;
				if (ELF32_R_TYPE(tmp_rel->r_info) == 1)
					*(uint32_t*)P = S + A;
				else
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
	struct modules *module = NULL;
	for (int i = 0; i < MAX_MODULES; i++)
	{
		if (modules[i].state == MOD_STATE_EMPTY)
		{
			modules[i].state = MOD_STATE_LOADING;
			module = &modules[i];
			break;
		}
	}
	if (module == NULL)
		return (-1);
	struct vfs_node *node = get_vfs_node_path(binary);
	if (node == 0x0)
		return (-1);
	char *content = (char*)kmalloc(node->size);
	if (!content)
		return (-1);
	ssize_t res = node->ops->read(node, content, node->size, 0);
	if (res == -1)
		return (kfree(content), -1);
	kfree(node);
	struct elf32_ehdr *elf = (struct elf32_ehdr*)content;
	if (check_binary(elf) == false)
		return (kfree(content), -1);

	uint32_t *tmp_array = kmalloc(sizeof(uint32_t) * elf->e_shnum);
	if (!tmp_array)
		return (kfree(content), -1);
	kmemset(tmp_array, 0, (sizeof(uint32_t) * elf->e_shnum));

	if (alloc_sections(content, elf, tmp_array) == -1)
		return (kfree(content), kfree(tmp_array), -1);

	if (realocation_symbols(content, elf, tmp_array) == -1)
		return (kfree(content), kfree(tmp_array), -1);

	if (search_init_function(content, elf, tmp_array, module) == -1)
		return (kfree(content), kfree(tmp_array), 1);

	if (module->init == NULL || module->cleanup == NULL)
		return (kfree(content), kfree(tmp_array), 1);
	int retur = module->init();
	if (retur != 0)
		return (kfree(content), kfree(tmp_array), 1);
	module->state = MOD_STATE_LIVE;
	kfree(content);
	return (0);
}

