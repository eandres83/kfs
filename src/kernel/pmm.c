#include <utils.h>
#include "multiboot.h"

#define MAX_PAGES 32768
#define PAGE_SIZE 4096

extern uint32_t _start;
extern uint32_t _end;

static uint32_t pmm_bitmap[MAX_PAGES];

static uint32_t total_ram_frames = 0;

// Marca una pagina como OCUPADA bit = 1
static void pmm_set_bit(uint32_t frame_idx)
{
	// frame_idx / 32 -> En que entero del array estoy
	// frame_idx % 32 -> En que bit dentro de ese entero estoy
	pmm_bitmap[frame_idx / 32] |= (1 << (frame_idx % 32));
}

// Marca una pagina como Libre bit = 0
static void pmm_unset_bit(uint32_t frame_idx)
{
	pmm_bitmap[frame_idx / 32] &= ~(1 << (frame_idx % 32));
}

// Comprobar si la pagina esta ocupada
int pmm_test_bit(uint32_t frame_idx)
{
	if ((pmm_bitmap[frame_idx / 32] & (1 << (frame_idx % 32))) != 0)
		return (1);
	return (0);
}

void	init_pmm(multiboot_info_t *mboot_info)
{
	// Poner todo a 1 (ocupado) por seguridad
	kmemset(pmm_bitmap, 0xFF, sizeof(pmm_bitmap));

	// Leer el mapa del GRUB y liberar lo que sea RAM valida
	multiboot_memory_map_t *entry = (multiboot_memory_map_t *)mboot_info->mmap_addr;
	uint32_t mmap_end_addr = mboot_info->mmap_addr + mboot_info->mmap_length;

	while ((uint32_t)entry < mmap_end_addr)
	{
		if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
		{
			uint64_t addr = entry->addr;
			uint64_t len = entry->len;

			for (uint64_t i = 0; i < len; i += PAGE_SIZE)
			{
				uint64_t current_addr = addr + i;
				// Indice de pagina
				uint32_t page_idx = current_addr / PAGE_SIZE;

				// Solo si el indice cabe en nuestro bitmap (seguridad para > 4GB)
				if (page_idx < (MAX_PAGES * 32))
				{
					pmm_unset_bit(page_idx);
					total_ram_frames++;
				}
			}
		}
		// Avanzar a la siguiente entrada del mapa
		entry = (multiboot_memory_map_t *)((uint32_t)entry + entry->size + sizeof(entry->size));
	}
	uint32_t kernel_start_page = ((uint32_t)&_start) / PAGE_SIZE;
	uint32_t kernel_end_page = ((uint32_t)&_end) / PAGE_SIZE;

	for (uint32_t i = kernel_start_page; i <= kernel_end_page; i++)
		pmm_set_bit(i);

	kprintf("PMM Initialized. Total RAM Pages: %d\n", total_ram_frames);
}

