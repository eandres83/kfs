#include "fs/mbr.h"

struct mbr_entry	*partition(uint32_t num_partition)
{
	struct mbr_entry *entry = kmalloc(sizeof(struct mbr_entry));
	if (!entry)
		return (kprintf("Error: kmalloc\n"), NULL);

	kmemset(entry, 0, sizeof(struct mbr_entry));
	uint8_t	buffer[512];
	ide_read_sector(0, buffer);

	kmemcpy(entry, ((char*)buffer + (446 + (num_partition * 16))), sizeof(struct mbr_entry));
	kprintf("mbr_entry lba_start -> %d\n", entry->lba_start);
	kprintf("mbr_entry nb_sector -> %d\n", entry->nb_sector);

	return (entry);
}

