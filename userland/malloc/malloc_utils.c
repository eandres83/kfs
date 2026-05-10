#include "../include/malloc.h"

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

// Handles LARGE allocations (> 1024 bytes)
// Allocates directly via mmap and links to g_heap.large_zone
void	*malloc_large(size_t size)
{
	t_block *new_block;
	size_t	total_size;

	total_size = size + BLOCK_META_SIZE;
	// Round up to multiple of page size
	// ((total + 4095) / 4096) * 4096
	total_size = ((total_size + PAGE_SIZE -1) / PAGE_SIZE) * PAGE_SIZE;

	new_block = (t_block *)request_memory(total_size);
	// Configure header
	new_block->size = total_size - BLOCK_META_SIZE; // Store payload capacity
	new_block->free = 0;
	new_block->next = g_heap.large_zone; // Insertamos al principio
	new_block->prev = NULL;

	// Si existe tenemos que actualizar su ->prev ahora tiene que ser new_block;
	// [ Bloque A ] <-----(prev)----- [ Bloque B]
	if (g_heap.large_zone)
		g_heap.large_zone->prev = new_block;

	// Y tambien actualizamos para que ahora la variable global apunte al nuevo bloque jefe.
	g_heap.large_zone = new_block;
	// devolver el puntero al payload
	return ((void *)((char *)new_block + BLOCK_META_SIZE));
}

// Wrapper for mmap to request raw memory from the OS.
void	*request_memory(size_t size)
{
	void *ptr;

	ptr = mmap(size);
	if (ptr == -1)
		return (NULL);
	return (ptr);
}

