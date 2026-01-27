#include <utils.h>
#include "multiboot.h"

#define MAX_PAGES 32768
#define PAGE_SIZE 4096

extern uint32_t _start;
extern uint32_t _end;

static uint32_t pmm_bitmap[MAX_PAGES];

static uint32_t total_ram_frames = 0;
static uint32_t used_frames = 0;

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

static	void	pmm_reserver_kernel(void)
{
	uint32_t start_page;
	uint32_t end_page;

	start_page = ((uint32_t)&_start) / PAGE_SIZE;
	end_page = ((uint32_t)&_end) / PAGE_SIZE;

	for (uint32_t i = start_page; i <= end_page; i++)
		pmm_set_bit(i);
}

static void	pmm_init_region(uint64_t addr, uint64_t len)
{
	uint64_t current_addr;
	uint64_t page_idx;

	// Iteramos por cada pagina dentro de la region
	for (uint64_t i = 0; i < len; i += PAGE_SIZE)
	{
		current_addr = addr + i;
		page_idx = current_addr / PAGE_SIZE;

		// Solo si el indice cabe en nuestro bitmap
		if (page_idx < (MAX_PAGES * 32))
		{
			pmm_unset_bit(page_idx);
			total_ram_frames++;
		}
	}

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
			pmm_init_region(entry->addr, entry->len);

		// Avanzar a la siguiente entrada del mapa
		entry = (multiboot_memory_map_t *)((uint32_t)entry + entry->size + sizeof(entry->size));
	}

	// Proteger el kernel
	pmm_reserver_kernel();

	kprintf("PMM Initialized. Total RAM Pages: %d\n", total_ram_frames);
}

void	*pmm_alloc_page()
{
	uint32_t	phys_address;
	uint32_t	frame_idx;

	for (uint32_t i = 0; i < MAX_PAGES; i++)
	{
		if (pmm_bitmap[i] == 0xFFFFFFFF)
			continue;

		for (uint32_t j = 0; j < 32; j++)
		{
			if ((pmm_bitmap[i] & (1 << j)) == 0)
			{
				frame_idx = (i * 32) + j;

				pmm_set_bit(frame_idx);
				used_frames++;

				phys_address = frame_idx * PAGE_SIZE;
				return ((void *)phys_address);
			}
		}
	}

	kprintf("PANIC: Out of memory\n");
	return (NULL);
}

void	pmm_free_page(void *p)
{
	uint32_t addr;
	uint32_t frame_idx;

	addr = (uint32_t)p;
	frame_idx = addr / PAGE_SIZE;

	if (frame_idx < (MAX_PAGES * 32))
	{
		pmm_unset_bit(frame_idx);
		used_frames--;
	}
	return ;
}

