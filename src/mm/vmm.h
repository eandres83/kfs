#ifndef PAGING_H
#define PAGING_H

#include <utils.h>
#include "multiboot.h"
#include "vmm_pte_pde.h"

typedef uint32_t virtual_addr;

#define PAGES_PER_TABLE 1024
#define PAGES_PER_DIR 	1024

// define 1024 entries per table - 0x3ff -> 1023
#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)

// page table
typedef struct page_table
{
	pt_entry m_entries[PAGES_PER_TABLE];
} 	page_table;

// page directory
typedef struct page_directory
{
	pd_entry m_entries[PAGES_PER_DIR];
} 	page_directory;

extern void load_page_directory(uint32_t s);
extern void enable_paging();

void	vmm_initialize();

#endif
