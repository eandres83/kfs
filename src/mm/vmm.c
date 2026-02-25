#include "vmm.h"
#include "vmm_pte_pde.h"

page_directory	*_cur_directory = 0;

uint32_t	_cur_pdbr = 0;

static bool vmm_switch_pdirectory(page_directory *dir)
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
	pt_entry_set_frame(e, (physical_address)p);
	pt_entry_add_attrib(e, PTE_PRESENT);

	return (true);
}

static void	vmm_free_page(pt_entry *e)
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
	pd_entry *e = &pd->m_entries[PD_INDEX((uint32_t) virt)];
	if ((*e & PTE_PRESENT) != PTE_PRESENT)
	{
		// page table not present, allocate it
		page_table *table = (page_table*)pmm_map_page();
		if (!table)
			PANIC("Failed to allocate new Page Table");
		kmemset(table, 0, sizeof(page_table));

		// create a new entry
		pd_entry* entry = &pd->m_entries[PD_INDEX((uint32_t)virt)];

		pd_entry_add_attrib(entry, PDE_PRESENT);
		pd_entry_add_attrib(entry, PDE_WRITABLE);
		pd_entry_set_frame(entry, (uint32_t)table);
	}
	// get physical table
	page_table *table = (page_table *)PAGE_PHYS_ADDRESS(e);
	// get page
	pt_entry *page = &table->m_entries[PT_INDEX((uint32_t) virt)];

	pt_entry_set_frame(page, (physical_address)phys);
	pt_entry_add_attrib(page, PTE_PRESENT);
	pt_entry_add_attrib(page, PTE_WRITABLE);

	reload_tlb(virt);
}

void	vmm_unmap_page(void *virt)
{
	page_directory *pd = _cur_directory;

	// get current directory
	pd_entry *e = &pd->m_entries[PD_INDEX((uint32_t)virt)];
	if ((*e & PDE_PRESENT) != PDE_PRESENT)
		return ;

	// get table
	page_table *table = (page_table *)PAGE_PHYS_ADDRESS(e);

	// get page in table
	pt_entry *page = &table->m_entries[PT_INDEX((uint32_t)virt)];

	// free page
	vmm_free_page((void*)page);

	reload_tlb(virt);
}

void	vmm_initialize()
{
	// allocate default page table
	page_table *table = (page_table *)pmm_map_page();
	if (!table)
		PANIC("Failed to allocate new Page Table");

	// allocate 3gb page table
	page_table *table2 = (page_table *)pmm_map_page();
	if (!table2)
		PANIC("Failed to allocate new Page Table");

	// clear page table
	kmemset(table, 0, sizeof(page_table));
	kmemset(table2, 0, sizeof(page_table));
	// 1st 4mb are identity mapped
	for (uint32_t i = 0, frame = 0x0, virt = 0x0000000; i < 1024; i++, frame += 4096, virt += 4096)
	{
		pt_entry page = 0;
		pt_entry_add_attrib(&page, PTE_PRESENT);
		pt_entry_add_attrib(&page, PTE_WRITABLE);
		pt_entry_set_frame(&page, frame);

		// add it to the page table
		table2->m_entries[PT_INDEX(virt)] = page;
	}

	// map 1mb to 3gb (where we are at)
	for (uint32_t i = 0, frame = 0x0, virt = 0xc0000000; i < 1024; i++, frame += 4096, virt += 4096)
	{
		pt_entry page = 0;
		pt_entry_add_attrib(&page, PTE_PRESENT);
		pt_entry_add_attrib(&page, PTE_WRITABLE);
		pt_entry_set_frame(&page, frame);

		table->m_entries[PT_INDEX(virt)] = page;
	}

	// mapear la ultima pagina de 3GB al buffer de video fisico
	pt_entry vga_page = 0;
	pt_entry_add_attrib((&vga_page), PTE_PRESENT);
	pt_entry_add_attrib((&vga_page), PTE_WRITABLE);
	pt_entry_set_frame(&vga_page, 0xB8000);
	table->m_entries[PT_INDEX(0xC03FF000)] = vga_page;

	// create defautl directory table
	page_directory* dir = (page_directory*)pmm_map_page();
	if (!dir)
		PANIC("Failed to allocate new Directory Table");
	kmemset(dir, 0, sizeof(page_directory));

	pd_entry *entry = &dir->m_entries[PD_INDEX(0xc0000000)];
	pd_entry_add_attrib(entry, PDE_PRESENT);
	pd_entry_add_attrib(entry, PDE_WRITABLE);
	pd_entry_set_frame(entry, (physical_address)table);

	pd_entry *entry2 = &dir->m_entries[PD_INDEX(0x00000000)];
	pd_entry_add_attrib(entry2, PDE_PRESENT);
	pd_entry_add_attrib(entry2, PDE_WRITABLE);
	pd_entry_set_frame(entry2, (physical_address)table2);

	_cur_pdbr = (physical_address)&dir->m_entries;

	// switch to our page directory
	vmm_switch_pdirectory(dir);

	enable_paging();
}

void	virt2phys(uint32_t virt)
{
	page_directory *pd = _cur_directory;
	pd_entry *pde = &pd->m_entries[PD_INDEX(virt)];

	kprintf("Page Directory Entry (PDE) Index: %d\n", PD_INDEX(virt));
	if (!pd_entry_is_present(*pde))
		return ;

	page_table *table = (page_table*)PAGE_PHYS_ADDRESS(pde);

	pt_entry *pte = &table->m_entries[PT_INDEX(virt)];
	kprintf("Page Table Entry (PTE) Index: %d\n", PT_INDEX(virt));
	if (!pt_entry_is_present(*pte))
		return ;

	uint32_t phys_frame = *pte & PTE_FRAME;
	uint32_t offset = virt & 0xFFF;
	uint32_t phys_address = phys_frame + offset;

	kprintf("Physical Frame: 0x%x\n", phys_frame);
	kprintf("Offset: 0x%x\n", offset);
	kprintf("EXACT PHYSICAL ADDRESS: 0x%x\n", phys_address);

	kprintf("Flags: PRESENT");
	if (pt_entry_is_writable(*pte))
		kprintf(" | WRITABLE");
	if (*pte & PTE_USER)
		kprintf(" | USER");
	kprintf("\n");
}

