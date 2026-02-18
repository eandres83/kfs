#include "vmm.h"
#include "vmm_pte_pde.h"

page_directory	*_cur_directory = 0;

uint32_t	_cur_pdbr = 0;

inline bool vmm_switch_pdirectory(page_directory *dir)
{
	if (!dir)
		return (false);

	_cur_directory = dir;
	load_page_directory(_cur_pdbr);
	return (true);
}

bool	vmm_alloc_page(pt_entry *e)
{
	void *p;

	p = pmm_map_page();
	if (!p)
		return (false);
	pt_entry_set_frame(e, (uint32_t)p);
	pt_entry_add_attrib(e, PTE_PRESENT);

	return (true);
}

void	vmm_free_page(pt_entry *e)
{
	void *p;

	p = (void *)pt_entry_frame(*e);
	if (p)
		pmm_free_page(p);

	pt_entry_del_attrib(e, PTE_PRESENT);
}

void	vmm_map_page(void *phys, void *virt)
{
	// get directory
	page_directory *pd = _cur_directory;

	// get page table
	pd_entry *e = &pd->m_entries[PAGE_DIRECTORY_INDEX((uint32_t) virt)];
	if ((*e & PTE_PRESENT) != PTE_PRESENT)
	{
		// page table not present, allocate it
		page_table *table = (page_table*)pmm_map_page();
		if (!table)
			return ;
		kmemset(table, 0, sizeof(page_table));

		// create a new entry
		pd_entry* entry = &pd->m_entries[PAGE_DIRECTORY_INDEX((uint32_t)virt)];

		pd_entry_add_attrib(entry, PDE_PRESENT);
		pd_entry_add_attrib(entry, PDE_WRITABLE);
		pd_entry_set_frame(entry, (uint32_t)table);
	}
	// get table
	page_table *table = (page_table *)PAGE_GET_PHYSICAL_ADDRESS(e);
	// get page
	pt_entry *page = &table->m_entries[PAGE_TABLE_INDEX((uint32_t) virt)];

	pt_entry_set_frame(page, (uint32_t)phys);
	pt_entry_add_attrib(page, PTE_PRESENT);
}

void	vmm_initialize()
{
	// allocate default page table
	page_table *table = (page_table *)pmm_map_page();
	if (!table)
		return;
	// allocate 3gb page table
	page_table *table2 = (page_table *)pmm_map_page();
	if (!table2)
		return;
	// clear page table
	kmemset(table, 0, sizeof(page_table));

	// 1st 4mb are identity mapped
	for (uint32_t i = 0, frame = 0x0, virt = 0x0000000; i < 1024; i++, frame += 4096, virt += 4096)
	{
		pt_entry page = 0;
		pt_entry_add_attrib(&page, PTE_PRESENT);
		pt_entry_set_frame(&page, frame);

		// add it to the page table
		table2->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	// map 1mb to 3gb (where we are at)
	for (uint32_t i = 0, frame = 0x100000, virt = 0xc0000000; i < 1024; i++, frame += 4096, virt += 4096)
	{
		pt_entry page = 0;
		pt_entry_add_attrib(&page, PTE_PRESENT);
		pt_entry_set_frame(&page, frame);

		table->m_entries[PAGE_TABLE_INDEX(virt)] = page;
	}

	// create defautl directory table
	page_directory* dir = (page_directory*)pmm_map_page();
	if (!dir)
		return;
	kmemset(dir, 0, sizeof(page_directory));

	pd_entry *entry = &dir->m_entries[PAGE_DIRECTORY_INDEX(0xc0000000)];
	pd_entry_add_attrib(entry, PDE_PRESENT);
	pd_entry_add_attrib(entry, PDE_WRITABLE);
	pd_entry_set_frame(entry, (uint32_t)table);

	pd_entry *entry2 = &dir->m_entries[PAGE_DIRECTORY_INDEX(0x00000000)];
	pd_entry_add_attrib(entry2, PDE_PRESENT);
	pd_entry_add_attrib(entry2, PDE_WRITABLE);
	pd_entry_set_frame(entry2, (uint32_t)table2);

	_cur_pdbr = (uint32_t)&dir->m_entries;

	// switch to our page directory
	vmm_switch_pdirectory(dir);

	enable_paging();
}

