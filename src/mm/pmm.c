#include <utils.h>
#include "multiboot.h"

#define MAX_PAGES 32768
#define PAGE_SIZE 4096

extern uint32_t _start;
extern uint32_t _end;

static uint32_t pmm_bitmap[MAX_PAGES];

static uint32_t total_ram_frames = 0;
static uint32_t used_frames = 0;

static inline void pmm_set_bit(uint32_t frame_idx)
{
	// frame_idx / 32 -> En que entero del array estoy
	// frame_idx % 32 -> En que bit dentro de ese entero estoy
	pmm_bitmap[frame_idx / 32] |= (1 << (frame_idx % 32));
}

static inline void pmm_unset_bit(uint32_t frame_idx)
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

	// we iterate through each page within the region
	for (uint64_t i = 0; i < len; i += PAGE_SIZE)
	{
		current_addr = addr + i;
		page_idx = current_addr / PAGE_SIZE;

		// only if the index fits in our bitmap
		if (page_idx < (MAX_PAGES * 32))
		{
			pmm_unset_bit(page_idx);
			total_ram_frames++;
		}
	}

}

void	init_pmm(multiboot_info_t *mboot_info)
{
	// set everything to 1 for security
	kmemset(pmm_bitmap, 0xFF, sizeof(pmm_bitmap));

	multiboot_memory_map_t *entry = (multiboot_memory_map_t *)mboot_info->mmap_addr;
	uint32_t mmap_end_addr = mboot_info->mmap_addr + mboot_info->mmap_length;

	while ((uint32_t)entry < mmap_end_addr)
	{
		if (entry->type == MULTIBOOT_MEMORY_AVAILABLE)
			pmm_init_region(entry->addr, entry->len);

		// continue to the next map entry
		entry = (multiboot_memory_map_t *)((uint32_t)entry + entry->size + sizeof(entry->size));
	}

	// protect the kernel
	pmm_reserver_kernel();

	kprintf("PMM Initialized. Total RAM Pages: %d\n", total_ram_frames);
}

// find the first free page in the bitmap
void	*pmm_map_page()
{
	uint32_t	phys_address;
	uint32_t	frame_idx;

	for (uint32_t i = 0; i < MAX_PAGES; i++)
	{
		// if page is full continue to the next one
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

