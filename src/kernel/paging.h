#ifndef PAGING_H
#define PAGING_H

#include <utils.h>
#include "multiboot.h"

typedef struct page_entry
{
	uint32_t present	: 1;
	uint32_t rw		: 1; // Read/Write
	uint32_t us		: 1; // User/Supervisor
	uint32_t pwt		: 1; // Write-Through
	uint32_t pcd		: 1; // Cache Disable
	uint32_t accessed	: 1;
	uint32_t dirty		: 1;
	uint32_t ps		: 1; // Page Size
	uint32_t glogal		: 1;
	uint32_t avail		: 3; // Available
	uint32_t frame		: 20; // Physical direction >> 12
} __attribute__((packed)) page_entry_t;

typedef struct page_table
{
	page_entry_t entries[1024];
} __attribute__((aligned(4096))) page_table_t;

// Asegura que empieze en una direccion multiplo de 4KB

typedef struct page_directory
{
	page_entry_t entries[1024];
} __attribute__((aligned(4096))) page_directory_t;

extern void load_page_directory(page_directory_t *s);
extern void enable_paging();

void	init_paging();

#endif
