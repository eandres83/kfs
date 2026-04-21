#include "ext2.h"

void test_ext2()
{
	uint8_t	buffer[1024];

	ide_read_sector(2, buffer);
	ide_read_sector(3, buffer);

	struct sb *superblock = (struct sb *)buffer;

	kprintf("El magic numer -> %d\n", superblock->magic_number);
}

