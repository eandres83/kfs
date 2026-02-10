/*
	init_paging() - Initializes the Virtual Memory Manager (VMM).

	This function sets up the permanent Page Directory and establishes
	the Higher Half mapping for the Kernel

	Rationale:
	Although boot.s set up a temporary table, we allocate a new clean
	directory from the PMM to ensure we have full control over the
	memory layout and flags.

	Note:
	This function MUST run with Identity Mapping active, as it accesses
	physical addresses returned by pmm_alloc_page() directly.
*/

#include "paging.h"

void	vmm_map_page()
{
	
}

void	init_paging()
{
	page_directory_t *pd = (page_directory_t *)pmm_map_page();
	kmemset(pd, 0, 4096);

	page_table_t *pt = (page_table_t *)pmm_map_page();
	kmemset(pt, 0, 4096);

	for (uint32_t i = 0; i < 1024; i++)
	{
		pt->entries[i].frame = i;
		pt->entries[i].present = 1;
		pt->entries[i].rw = 1;
	}

	/*
		KERNEL MAPPING (Higher Half):
		Map the Page Table 'pt' (convering 0-4MB Physical) to
		Directory Entry 768 (corresponding to 3GB Virtual).
		Flags: Present | RW | Supervisor (User bit = 0)
	*/
	pd->entries[768].frame = (uint32_t)pt >> 12;
	pd->entries[768].present = 1;
	pd->entries[768].rw = 1;
	pd->entries[768].us = 0; // Protected from Ring 3

	/*
		VGA MEMORY MAPPING:
		We map the last page of the first table (Idx 1023) to the
		physical VGA buffer (0xB8000).
		This allows the kernel to access video memory at 0xC03FF000.
	*/
	pt->entries[1023].frame = 0xB8000 >> 12;
	pt->entries[1023].present = 1;
	pt->entries[1023].rw = 1;

	pd->entries[0].frame = (uint32_t)pt >> 12;
	pd->entries[0].present = 1;
	pd->entries[0].rw = 1;

	load_page_directory(pd);
	enable_paging();
}

