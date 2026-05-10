#ifndef MALLOC_H
#define MALLOC_H

#define TINY_MAX_SIZE 128
#define SMALL_MAX_SIZE 1024

// ALIGN(x): Aligns x to the next multiple of 16 bytes (128 bits)
// BLOCK_META_SIZE: Size of t_block struct aligned to 16 bytes
#define ALIGN(x) (((x) + 15) & ~15)
#define BLOCK_META_SIZE ALIGN(sizeof(t_block))
#define PAGE_SIZE getpagesize()

#include "../minilib.h"

// Describes a memory block (header). The payload follows immediately after.
typedef struct s_block
{
	size_t		size; // 8 bytes
	struct	s_block	*next; // 8 bytes
	struct	s_block	*prev; // 8 bytes
	int 		free; // 4 bytes
}	t_block; // 28 bytes
// Total aligned to 32 with padding

// Holds pointers to the start of each zone type.
typedef struct s_heap
{
	t_block	*tiny_zone;
	t_block	*small_zone;
	t_block *large_zone;
}	t_heap;

extern t_heap g_heap;

// Core Functions
void	free(void *ptr);
void	*malloc(size_t size);
void	*realloc(void *ptr, size_t size);

// Utils
void	split_block(t_block *block, size_t size);
void	*request_memory(size_t size);
void	*malloc_large(size_t size);
t_block	*find_free_block(t_block **list, size_t size);

#endif
