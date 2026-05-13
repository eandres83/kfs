#include "malloc.h"

t_heap g_heap = {0};

// Creates a single large free block covering the entire zone.
static	t_block *init_zone(size_t zone_size)
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
static	size_t	calc_zone_size(size_t max_block_size)
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
static	t_block *extend_zone(t_block *last, size_t zone_type_size)
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
static	t_block *handle_zone_allocation(t_block **head, size_t size, size_t max_size)
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

void	*malloc(size_t size)
{
	void	*ptr;
	size_t	aligned_size;
	t_block *block;
	
	if (size <= 0)
		return (NULL);

	aligned_size = ALIGN(size);
	block = NULL;
	if (aligned_size <= TINY_MAX_SIZE)
		block = handle_zone_allocation(&g_heap.tiny_zone, aligned_size, TINY_MAX_SIZE);
	else if (aligned_size <= SMALL_MAX_SIZE)
		block = handle_zone_allocation(&g_heap.small_zone, aligned_size, SMALL_MAX_SIZE);
	else
		return (malloc_large(aligned_size));
	if (!block)
		return (NULL);
	split_block(block, aligned_size);
	block->free = 0;
	ptr = (void *)((char *)block + BLOCK_META_SIZE);

	return (ptr);
}

