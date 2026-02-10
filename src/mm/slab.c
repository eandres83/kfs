#include "slab.h"
#include "multiboot.h"

// Wrapper for pmm_alloc_page to request raw memory
// void	*request_memory(size_t size)
//{
//	size_t 	num_pages;
//	void	*ptr;
//
//	num_pages = (size + 4095) / 4096;
//
//	ptr = pmm_alloc_page();
//	if (!ptr)
//		return (NULL);
//	return (ptr);
//}

// Iterates through the list to find the firts free block that fits the size
// Return NULL if no suitable block is found
t_block	*find_free_block(t_block **list, size_t size)
{
	t_block	*current;

	current = *list;

	while (current != NULL)
	{
		if (current->free && current->size >= size)
			return (current);

		current = current->next;
	}
	return (NULL);
}

// Splits a block into two:
// 1. The requested block (occupied)
// 2. A new remainder block (free)
// Only splits if there is enought space for a new header + minimal payload
// [ Bloque A ] <-----(prev/next)-----> [ Bloque B ]
void split_block(t_block *block, size_t size)
{
	t_block *new_block;

	// Ensure thre's enough space for a new block header + alignment padding
	if (block->size < size + BLOCK_META_SIZE + ALIGN(1))
		return ;

	// Calculate address of the new block
	new_block = (t_block *)((char *)block + BLOCK_META_SIZE + size);

	// Setup new block metadata
	new_block->size = block->size - size - BLOCK_META_SIZE;
	new_block->next = block->next;
	new_block->prev = block;
	new_block->free = 1;

	// Update current block
	block->size = size;
	block->next = new_block;

	// Update the backward link of the next block
	if (new_block->next)
		new_block->next->prev = new_block;
}

// Creates a single large free block covering the entire zone.
static t_block *init_zone(size_t zone_size)
{
	void 	*zone_ptr;
	t_block *first_block;

	zone_ptr = request_memory(zone_size);
	if (!zone_ptr)
		return (NULL);

	first_block = (t_block *)zone_ptr;

	first_block->size = zone_size - BLOCK_META_SIZE;
	first_block->next = NULL;
	first_block->prev = NULL;
	first_block->free = 1;

	return (first_block);
}

// Must accommodate at least 100 blocks of max_block_size to minimize mmap calls.
// Returns a multiple of getpagesize().
static size_t	calc_zone_size(size_t max_block_size)
{
	size_t min_size_needed;
	size_t zones_size;

	// We calculate how much space 100 blocks occupy (Data + Header)
	min_size_needed = (max_block_size + BLOCK_META_SIZE) * 100;

	// We round up to the next multiple of page_size
	zones_size = ((min_size_needed + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;

	return (zones_size);
}

// Extends a zone chain by allocating a new zone and linking it to the last block.
static t_block *extend_zone(t_block *last, size_t zone_type_size)
{
	size_t	zone_size;
	t_block	*new_zone;

	zone_size = calc_zone_size(zone_type_size);
	new_zone = init_zone(zone_size);
	if (!new_zone)
		return (NULL);
	if (last)
		last->next = new_zone;

	new_zone->prev = last;
	return (new_zone);
}

// Helper to handle TINY and SMALL allocations.
// 1. Checks if zone exists
// 2. Tries to find a free block
// 3. Extends zone if no space is found
t_block *handle_zone_allocation(t_block **head, size_t size, size_t max_size)
{
	t_block *block;
	t_block	*cursor;

	if (!*head)
	{
		*head = init_zone(calc_zone_size(max_size));
		if (!*head)
			return (NULL);
	}
	block = find_free_block(head, size);
	if (!block)
	{
		cursor = *head;
		while (cursor && cursor->next)
			cursor = cursor->next;
		block = extend_zone(cursor, max_size);
	}
	return (block);
}
