#include <utils.h>
#include "pmm.h"

extern uint32_t _start;
extern uint32_t _end;

static uint32_t pmm_bitmap[MAX_PAGES];
static uint32_t last_frame = 0;

static uint32_t total_ram_frames = 0;

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

	uint32_t start_phys;
	uint32_t end_phys;

	start_phys = ((uint32_t)&_start) - 0xC0000000;
	end_phys = ((uint32_t)&_end) - 0xC0000000;

	start_page = start_phys / PAGE_SIZE;
	end_page = end_phys / PAGE_SIZE;

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
	last_frame = 0;
	total_ram_frames = 0;

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

	pmm_set_bit(0);

	// protect the kernel
	pmm_reserver_kernel();

	kprintf("PMM Initialized. Total RAM Pages: %d\n", total_ram_frames);
}

// find the first free page in the bitmap (next-fit)
void	*pmm_map_page()
{
	uint32_t	phys_address;
	uint32_t	frame_idx;

	for (uint32_t i = last_frame; i < MAX_PAGES; i++)
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

				phys_address = frame_idx * PAGE_SIZE;
				return ((void *)phys_address);
			}
		}
		if (last_frame >= MAX_PAGES)
			last_frame = 0;
	}

	PANIC("Out of memory");
}

void	pmm_free_page(void *p)
{
	uint32_t addr;
	uint32_t frame_idx;

	addr = (uint32_t)p;
	frame_idx = addr / PAGE_SIZE;

	if (frame_idx < (MAX_PAGES * 32))
		pmm_unset_bit(frame_idx);
	return ;
}

void	meminfo()
{
	uint32_t used_frames = 0;
	uint32_t free_frames = 0;
	uint32_t total_frames = total_ram_frames;

	for (uint32_t i = 0; i < (total_frames / 32); i++)
	{
		for (uint32_t bit = 0; bit < 32; bit++)
		{
			if ((i * 32) + bit >= total_frames)
				break;
			if (pmm_bitmap[i] & (1 << bit))
				used_frames++;
			else
				free_frames++;
		}
	}

	uint32_t total_kb = (total_frames * PAGE_SIZE) / 1024;
	uint32_t used_kb = (used_frames * PAGE_SIZE) / 1024;
	uint32_t free_kb = (free_frames * PAGE_SIZE) / 1024;

	kprintf("--- MEMORY INFO ---\n");
	kprintf("Total RAM: %d KB (%d pages)\n", total_kb, total_frames);
	kprintf("Used RAM : %d KB (%d pages)\n", used_kb, used_frames);
	kprintf("Free RAM : %d KB (%d pages)\n", free_kb, free_frames);
}

