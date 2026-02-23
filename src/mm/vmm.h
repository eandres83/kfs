#ifndef PAGING_H
#define PAGING_H

#include <utils.h>
#include "pmm.h"
#include "vmm_pte_pde.h"

#define PAGES_PER_TABLE 1024
#define TABLE_PER_DIR 	1024

// define 1024 entries per table - 0x3ff -> 1023
#define PD_INDEX(x) (((x) >> 22) & 0x3ff)
#define PT_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_PHYS_ADDRESS(x) ((*x) & ~0xfff)

typedef uint32_t virtual_addr;
typedef uint32_t physical_address;

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
void	vmm_map_page(void *phys, void *virt);
void	vmm_unmap_page(void *virt);
void	virt2phys(uint32_t virt);

#endif
