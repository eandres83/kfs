#include "vmm.h"
#include "task/task.h"

static page_directory	*_cur_directory = (page_directory*)0xFFFFF000;

void	vmm_load_process_directory(void *pd)
{
	if (!pd)
		return ;
	load_page_directory((uint32_t)pd);
}

bool	vmm_alloc_page(pt_entry *e)
{
	void *p;

	p = pmm_map_page();
	if (!p)
		return (false);
	pt_entry_set_frame(e, (physical_addr)p);
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

void	vmm_remove_mapping(void *virt)
{
	page_directory *pd = _cur_directory;
	pd_entry *e = &pd->m_entries[PD_INDEX((uint32_t)virt)];

	if ((*e & PDE_PRESENT) != PDE_PRESENT)
		return ;

	page_table *table = (page_table*)(0xFFC00000 + (PD_INDEX((uint32_t)virt) * PAGE_SIZE));
	pt_entry *page = &table->m_entries[PT_INDEX((uint32_t)virt)];

	pt_entry_del_attrib(page, PTE_PRESENT);
	pt_entry_set_frame(page, 0);

	reload_tlb(virt);
}

void	vmm_map_page(void *phys, void *virt, bool is_user)
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

		// create a new entry
		pd_entry* entry = &pd->m_entries[PD_INDEX((uint32_t)virt)];

		pd_entry_add_attrib(entry, PDE_PRESENT);
		pd_entry_add_attrib(entry, PDE_WRITABLE);
		if (is_user)
			pd_entry_add_attrib(entry, PDE_USER);
		pd_entry_set_frame(entry, (uint32_t)table);

		page_table *virt_table = (page_table*)(0xFFC00000 + (PD_INDEX((uint32_t)virt) * PAGE_SIZE));
		kmemset(virt_table, 0, sizeof(page_table));
	}
	// get physical table
	page_table *table = (page_table *)(0xFFC00000 + (PD_INDEX((uint32_t)virt) * PAGE_SIZE));
	// get page
	pt_entry *page = &table->m_entries[PT_INDEX((uint32_t) virt)];

	pt_entry_set_frame(page, (physical_addr)phys);
	pt_entry_add_attrib(page, PTE_PRESENT);
	pt_entry_add_attrib(page, PTE_WRITABLE);
	if (is_user)
		pt_entry_add_attrib(page, PTE_USER);

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
	page_table *table = (page_table *)(0xFFC00000 + (PD_INDEX((uint32_t)virt) * PAGE_SIZE));

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
		PANIC("Failed to allocate new Page Directory");
	kmemset(dir, 0, sizeof(page_directory));

	pd_entry *recursive_entry = &dir->m_entries[1023];
	pd_entry_set_frame(recursive_entry, (physical_addr)dir);
	pd_entry_add_attrib(recursive_entry, PDE_PRESENT);
	pd_entry_add_attrib(recursive_entry, PDE_WRITABLE);

	pd_entry *entry2 = &dir->m_entries[PD_INDEX(0x00000000)];
	pd_entry_add_attrib(entry2, PDE_PRESENT);
	pd_entry_add_attrib(entry2, PDE_WRITABLE);
	pd_entry_add_attrib(entry2, PDE_USER);
	pd_entry_set_frame(entry2, (physical_addr)table2);

	pd_entry *entry_kernel = &dir->m_entries[PD_INDEX(0xC0000000)];
	pd_entry_add_attrib(entry_kernel, PDE_PRESENT);
	pd_entry_add_attrib(entry_kernel, PDE_WRITABLE);
	pd_entry_add_attrib(entry_kernel, PDE_USER);
	pd_entry_set_frame(entry_kernel, (physical_addr)table);

	// cargra el nuevo directory to cr3
	load_page_directory((uint32_t)dir);

	enable_paging();
}

void copy_parent_memory(struct proc *proc)
{
	create_memory_process(proc);
	page_directory *dir = _cur_directory;

	void *virt1 = (void*)0xFFBFE000;
	vmm_map_page(proc->pd, virt1, true);
	page_directory *pd_chil = (page_directory*)virt1;

	for (int i = 1; i < 768; i++)
	{
		if (pd_entry_is_present(dir->m_entries[i]))
		{
			if (!pd_entry_is_present(pd_chil->m_entries[i]))
			{
				void *phys = pmm_map_page();
				if (!phys)
					return ;
				void *virt2 = (void*)0xFFBFF000;
				vmm_map_page(phys, virt2, true);
				kmemset(virt2, 0, sizeof(page_table));
				vmm_remove_mapping(virt2);

				pd_entry_set_frame(&pd_chil->m_entries[i], (uint32_t)phys);
				pd_entry_add_attrib(&pd_chil->m_entries[i], PDE_PRESENT);
				pd_entry_add_attrib(&pd_chil->m_entries[i], PDE_WRITABLE);
				pd_entry_add_attrib(&pd_chil->m_entries[i], PDE_USER);
			}
			physical_addr tabla_fisica_hijo = pt_entry_frame(pd_chil->m_entries[i]);
			void *virt3 = (void*)0xFFBFD000;
			vmm_map_page((void*)tabla_fisica_hijo, virt3, true);
			page_table *tabla_hijo = (page_table*)virt3;

			page_table *pt_parent = (page_table*)(0xFFC00000 + (i * 4096));
			for (int j = 0; j < 1024; j++)
			{
				if (pt_entry_is_present(pt_parent->m_entries[j]))
				{
					void *phys = pmm_map_page();
					if (!phys)
						return ;
					void *virt4 = (void*)0xFFBFC000;
					vmm_map_page(phys, virt4, true);

					// reconstruir la direccion virtual donde esta la info del padre
					uint32_t dir_ind = i << 22;
					uint32_t tab_ind = j << 12;
					uint32_t src = dir_ind | tab_ind;
					kmemcpy(virt4, (void*)src, 4096);
					vmm_remove_mapping(virt4);

					pt_entry nueva_entrada = 0;
					pt_entry_set_frame(&nueva_entrada, (uint32_t)phys);
					pt_entry_add_attrib(&nueva_entrada, PTE_PRESENT);
					pt_entry_add_attrib(&nueva_entrada, PTE_WRITABLE);
					pt_entry_add_attrib(&nueva_entrada, PTE_USER);

					tabla_hijo->m_entries[j] = nueva_entrada;
				}
			}
			vmm_remove_mapping(virt3);
		}
	}
	vmm_remove_mapping(virt1);
}

void create_memory_process(struct proc *proc)
{
	// get page directory
	void *phys = pmm_map_page();
	if (!phys)
		return ;
	void *virt = (void*)0xFFBFF000;
	vmm_map_page(phys, virt, true);

	page_directory *dir_virt = (page_directory*)virt;
	kmemset(dir_virt, 0, (sizeof(uint32_t) * 768));

	for (int i = 768; i < 1023; i++)
	{
		dir_virt->m_entries[i] = _cur_directory->m_entries[i];
	}

	// recursive paging
	pd_entry *recursive_entry = &dir_virt->m_entries[1023];
	pd_entry_set_frame(recursive_entry, (physical_addr)phys);
	pd_entry_add_attrib(recursive_entry, PDE_PRESENT);
	pd_entry_add_attrib(recursive_entry, PDE_WRITABLE);

	proc->pd = phys;
	vmm_remove_mapping(virt);
}

void	free_page_directory(void *pagedir)
{
	void *phys_pd = pagedir;
	page_directory *pd = (page_directory*)0xFFBFE000;
	vmm_map_page(phys_pd, (void*)pd, true);

	for (int i = 0; i < 768; i++)
	{
		if (pd_entry_is_present(pd->m_entries[i]))
		{
			void *virt = (void*)0xFFBFF000;
			void *phys = (void*)pt_entry_frame(pd->m_entries[i]);
			vmm_map_page(phys, virt, true);
			page_table *pt = (page_table*)virt;
			for (int j = 0; j < 1024; j++)
			{
				if (pt_entry_is_present(pt->m_entries[j]))
					vmm_free_page(&pt->m_entries[j]);
			}
			vmm_remove_mapping(virt);
			pmm_free_page(phys);
		}
	}
	vmm_remove_mapping((void*)pd);
	pmm_free_page(pagedir);
}

void	virt2phys(uint32_t virt)
{
	page_directory *pd = _cur_directory;
	pd_entry *pde = &pd->m_entries[PD_INDEX(virt)];

	kprintf("Page Directory Entry (PDE) Index: %d\n", PD_INDEX(virt));
	if (!pd_entry_is_present(*pde))
		return ;

	page_table *table = (page_table *)(0xFFC00000 + (PD_INDEX((uint32_t)virt) * PAGE_SIZE));

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

