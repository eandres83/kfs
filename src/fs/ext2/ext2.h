#ifndef EXT2_H
#define EXT2_H

#include <utils.h>
#include "drivers/ide/ide.h"

struct ext2_vfs
{
	
};

struct sb
{
	uint32_t	inodes_per_block; // total number of inodes in file system
	uint32_t	number_blocks; // total number of blocks in file system
	uint32_t	block_for_superuser; // number of blocks reserved for superuser
	uint32_t	unallocated_blocks; // total number of unallocated block
	uint32_t	unallocated_inodes; // total number of unallocated inodes
	uint32_t	block_nb_in_sb; // block number of the block containing the sb
	uint32_t	block_size; // number to shift 1024 to the left by to obtain the block size
	uint32_t	fragment_size; // the same but for fragment size
	uint32_t	blokcs_per_group; // number of blocks in each block group
	uint32_t	fragment_per_group; // number of fragments in each block group
	uint32_t	inodes_per_gorup; // number of inodes in each block group
	uint32_t	last_mount; // last mount time
	uint32_t	last_written; // last written time
	uint16_t	nb_mounted; // number of times the volume has been mounted
	uint16_t	nb_allowed_mounts; // number of mounts alloweb before a consistecy check
	uint16_t	magic_number; // ext2 signature
	uint16_t	fs_state; // file system state
	uint16_t	error_to_do; // what to do when an error is detected
	uint16_t	version_minor; // minor portion of version
	uint32_t	time_last_check; // POSIX time of last consistency check
	uint32_t	time_forced_check; // interval between forced consistency checks
	uint32_t	fs_id; // operating system id from which the filesystem on this volume was created
	uint32_t	version_major; // major portion of version
	uint16_t	user_id; // user id that can use reserved blocks
	uint16_t	group_id; // group id that can use reserved blocks
};

void test_ext2();

#endif
