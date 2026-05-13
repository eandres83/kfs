#include "malloc.h"

// Remove a block form the LARGE zone list and return mamory to the OS.
// 1. Unlinks the block from the global double-linked list
// 2. Calculates the total size originally allocated (aligned to PAGE_SIZE)
// 3. Calls munmap to release the memory back to the kernel.
static	void	free_large(t_block *block)
{
	size_t total_size;
	size_t size;

	// Unlink from the list
	if (block->prev)
		block->prev->next = block->next;
	else
		g_heap.large_zone = block->next;

	if (block->next)
		block->next->prev = block->prev;

	// Calculate original mmap size
	total_size = block->size + BLOCK_META_SIZE;
	size = ((total_size + PAGE_SIZE -1) / PAGE_SIZE) * PAGE_SIZE;

	// Return memory to OS
	if (munmap((void *)block, size) == -1)
		return ;
}

// Merges the current block with its next neighbor
// Absorbs the next block's metadata size and payload into the current block
static	void	merge_block(t_block *block)
{
	block->size = block->size + BLOCK_META_SIZE + block->next->size;
	block->next = block->next->next;

	// Update the backward link of the new next block
	if (block->next)
		block->next->prev = block;
}

// Check neighbors (next and prev) to see if they are free
// If yes, merges them into a single large block
// This is critical to prevent heap fragmentation
static	void	coalising(t_block *block)
{
	if (block->next && block->next->free == 1)
		merge_block(block);
	if (block->prev && block->prev->free == 1)
		merge_block(block->prev);
}

// Releases the memory space pointed to by ptr
// 1. Validate input pointer
// 2. Retrieves the block metadata
// 3. Identifies block type
// 4. Marks as free and coalesces (TINY/SMALL) or munmaps (LARGE)
void	free(void *ptr)
{
	t_block *block;

	if (ptr == NULL)
		return ;

	// Pointer arithmetic to find the Metadata Header
	block = (t_block *)((char *)ptr - BLOCK_META_SIZE);

	// Dispatcher
	if (block->size <= SMALL_MAX_SIZE)
	{
		block->free = 1;
		coalising(block);
	}
	else
		free_large(block);
}

void	*realloc(void *ptr, size_t size)
{
	t_block *block;
	void	*new_ptr;
	size_t	copy_size;

	if (ptr == NULL)
		return (malloc(size));

	if (size == 0)
		return (free(ptr), NULL);

	block = (t_block *)((char *)ptr - BLOCK_META_SIZE);

	// If the current block is large enough, we don't need to move data.
	// We try to split the block to return unused memory to the system.
	if (block->size >= ALIGN(size) && block->size <= SMALL_MAX_SIZE)
	{
		split_block(block, ALIGN(size));
		return (ptr);
	}

	new_ptr = malloc(size);
	if (!new_ptr)
		return (NULL);

	// calculate safe copy size (min of old vs new size)
	if (block->size < size)
		copy_size = block->size;
	else
		copy_size = size;

	memcpy(new_ptr, ptr, copy_size);
	free(ptr);

	return (new_ptr);
}

