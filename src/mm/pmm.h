#ifndef PMM_H
#define PMM_H

#include <utils.h>

#define MAX_PAGES 32768
#define PAGE_SIZE 4096

#define MULTIBOOT_MEMORY_AVAILABLE		1
#define MULTIBOOT_MEMORY_RESERVED		2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE	3
#define MULTIBOOT_MEMORY_NVS			4
#define MULTIBOOT_MEMORY_BADRAM			5

typedef struct multiboot_mmap_entry
{
	uint32_t size;
	uint64_t addr;
	uint64_t len;
	uint32_t type;
} __attribute__((packed)) multiboot_memory_map_t;

typedef struct multiboot_info 
{
	uint32_t flags;
	uint32_t mem_lower;
	uint32_t mem_upper;

	uint32_t boot_device;
	uint32_t cmdline;
	uint32_t mods_count;
	uint32_t mods_addr;

	// simbolos del kernel (ELF)
	union
	{
		struct
		{
			uint32_t tabsize;
			uint32_t strsize;
			uint32_t addr;
			uint32_t reserved;
		} aout_sym;
		struct
		{
			uint32_t num;
			uint32_t size;
			uint32_t addr;
			uint32_t shndx;
		} elf_sec;
	} u;

	uint32_t mmap_length; // tamano total del buffer del mapa
	uint32_t mmap_addr; // Direccion fisica donde empieza el array de mmap_entry

} multiboot_info_t;

void	init_pmm(multiboot_info_t *mboot_info);
void	*pmm_map_page();
void	pmm_free_page(void *p);

#endif
