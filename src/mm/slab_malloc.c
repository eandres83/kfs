#include "slab.h"

t_heap g_heap = {0};

// Handles LARGE allocations (> 1024 bytes)
// Allocates directly via mmap and links to g_heap.large_zone
static void	*malloc_large(size_t size)
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

// 1. Aligns the requested size
// 2. Routes the request to TINY, SMALL, or LARGE handlers
// 3. Split the block if it's too bit (for TINY/SMALL)
// 4. Return pointer to the payload
void	*kmalloc(size_t size)
{
	void	*ptr;
	size_t	aligned_size;
	t_block *block;
	
	if (size <= 0)
		return (NULL);

	// El size que me piden lo tengo que alinear a un multiplo de 16,
	// para mantener la alineacion de memoria (system alignment) de 64 bits y evitar
	// fallos en CPU. Si me piden 10 lo redondeo a 16.
	aligned_size = ALIGN(size);
	block = NULL;

	// Si es tiny o small, compruebo si ya he pre asignado un espacio para no tener que pedir todo el rato espacio
	// ahora de la zona pre asigned que es grande tengo que cojer solo el tamano que me han pedido,
	// la zona suele medir 16KB, si el user solo me pide 10 bytes tengo que splitearlo.
	if (aligned_size <= TINY_MAX_SIZE)
		block = handle_zone_allocation(&g_heap.tiny_zone, aligned_size, TINY_MAX_SIZE);
	else if (aligned_size <= SMALL_MAX_SIZE)
		block = handle_zone_allocation(&g_heap.small_zone, aligned_size, SMALL_MAX_SIZE);
	else
		return (malloc_large(aligned_size));
	// Cuando ya tienes la zona la tienes que splitear para devolver solo el tamano que te han pedido.
	// una tiny zone puede ser 16KB y el user solo te ha pedido 10 bytes,
	// hacer el split y devuelves el puntero al payload de ese bloque saltandote los metadatos del principio.
	if (!block)
		return (NULL);

	split_block(block, aligned_size);
	block->free = 0;
	ptr = (void *)((char *)block + BLOCK_META_SIZE);

//	kdebug("el valor de un malloc -> 0x%x\n", ptr);
	return (ptr);
}

