#ifndef KMALLOC_H
#define KMALLOC_H

#include <stddef.h>
#define PAGE_SIZE 4096

void	*kmalloc(size_t size);
void	kfree(void *ptr);
void	*krealloc(void *ptr, size_t size);
size_t	ksize(void *ptr);

#endif
