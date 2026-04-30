#ifndef MBR_H
#define MBR_H

#include <utils.h>
#include "drivers/ide/ide.h"

// mbr = Master Boot Record
struct mbr_entry
{
	uint8_t		status;
	uint8_t		first_chs[3]; // chs address of partition start
	uint8_t		type; // partition type
	uint8_t		last_chs[3]; // chs address of lsat partition sector
	uint32_t	lba_start; // lba of partition start
	uint32_t	nb_sector; // number of sectors in partition
}__attribute__((packed));

struct mbr_entry	*partition(uint32_t num_partition);

#endif
