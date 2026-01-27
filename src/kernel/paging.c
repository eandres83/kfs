#include "paging.h"

void	init_paging()
{
	page_directory_t *pd = (page_directory_t *)pmm_alloc_page();
	kmemset(pd, 0, 4096);

	page_table_t *pt = (page_table_t *)pmm_alloc_page();
	kmemset(pt, 0, 4096);

	for (uint32_t i = 0; i < 1024; i++)
	{
		pt->entries[i].frame = i;
		pt->entries[i].present = 1;
		pt->entries[i].rw = 1;
	}

	pd->entries[0].frame = (uint32_t)pt >> 12;
	pd->entries[0].present = 1;
	pd->entries[0].rw = 1;

	load_page_directory(pd);
	enable_paging();
}

