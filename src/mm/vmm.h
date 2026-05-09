#ifndef PAGING_H
#define PAGING_H

#include <utils.h>
#include "pmm.h"
#include "vmm_pte_pde.h"

#define PAGES_PER_TABLE 1024
#define TABLE_PER_DIR 	1024

// Page Directory (10bits) -> Page Table (10bits) -> offset (12bits)
// Bits 31 to 22	      21 to 12               11 to 0

// Move 22 bits to the right to get 10 higher bits (directory)
#define PD_INDEX(x) (((x) >> 22) & 0x3ff)
// Move 12 bits to ignore the offset (table)
#define PT_INDEX(x) (((x) >> 12) & 0x3ff)
// sets the lowest 12 bits to 0 to ignore the offset and obtain the actual physical address
#define PAGE_PHYS_ADDRESS(x) ((*x) & ~0xfff)

typedef uint32_t virtual_addr;
typedef uint32_t physical_addr;

// page table
typedef struct page_table
{
	pt_entry m_entries[PAGES_PER_TABLE];
} 	page_table;

// page directory
typedef struct page_directory
{
	pd_entry m_entries[TABLE_PER_DIR];
} 	page_directory;

extern void load_page_directory(uint32_t s);
extern void enable_paging();
extern void reload_tlb(void *virt);

void	vmm_initialize();
void	vmm_map_page(void *phys, void *virt, bool is_user);
void	vmm_unmap_page(void *virt);
void	virt2phys(uint32_t virt);
void	vmm_remove_mapping(void *virt);

// process function
struct proc;
void	create_memory_process(struct proc *proc);
void	vmm_load_process_directory(void *pd);
void 	copy_parent_memory(struct proc *proc);
void	free_page_directory(void *pagedir);

#endif
