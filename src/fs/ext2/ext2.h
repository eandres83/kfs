#ifndef EXT2_H
#define EXT2_H

#include <utils.h>
#include <kmalloc.h>
#include "drivers/ide/ide.h"
#include "fs/vfs/vfs.h"
#include "fs/mbr.h"
#include "arch/i386/lib/uaccess.h"

// superblock data struct
struct ext2_sb
{
	uint32_t	inodes_per_block; // total number of inodes in file system
	uint32_t	nb_block; // total number of blocks in file system
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
	uint16_t	magic_nb; // ext2 signature
	uint16_t	fs_state; // file system state
	uint16_t	error_to_do; // what to do when an error is detected
	uint16_t	version_minor; // minor portion of version
	uint32_t	time_last_check; // POSIX time of last consistency check
	uint32_t	time_forced_check; // interval between forced consistency checks
	uint32_t	fs_id; // operating system id from which the filesystem on this volume was created
	uint32_t	version_major; // major portion of version
	uint16_t	user_id; // user id that can use reserved blocks
	uint16_t	group_id; // group id that can use reserved blocks
	uint32_t	first_inode;
	uint16_t	inode_size;
}__attribute__((packed));

// block group descriptor
struct ext2_block
{
	uint32_t	addr_block_bitmap; // block address of block usabe bitmap
	uint32_t	addr_inode_bitmap; // block address of inode usage bitmap
	uint32_t	addr_inode_table; // starting block address of inode table
	uint16_t	unallocated_blocks; // number of unallocated blocks in group
	uint16_t	unallocated_inodes; // number of unallocated inodes in group
	uint16_t	nb_directories; // number of directories in group
	uint8_t		unused_padding[14]; // padding
}__attribute__((packed));

// inode data strcutr
struct ext2_inode
{
	uint16_t	type_permisi; // type and permissions
	uint16_t	user_id;
	uint32_t	lower_size; // lower 32 bits of size in bytes
	uint32_t	last_access_time;
	uint32_t	creation_time;
	uint32_t	last_mod_time;
	uint32_t	delete_time;
	uint16_t	group_id;
	uint16_t	nb_hard_links; // Count of hard links (directory entries) to this inode
	uint32_t	nb_disk_sector; // Count of disk sectors (not Ext2 blocks) in use by this inode
	uint32_t	flags;
	uint32_t	os_value1; // Operating Sistem specific value 1
	uint32_t	direct_block[12]; // Direct Block Pointer
	uint32_t	singly_indirect_block; // Singly indirect block pointer
	uint32_t	doubly_indirect_block; // Double indirect block pointer
	uint32_t	triply_indirect_block; // Triply indirect block pointer
	uint32_t	generation_nb; // Generation number
	uint32_t	ext_atribute_block; // Extended attribute block
	uint32_t	uper_size; // upper 32 bits of file size
	uint32_t	block_addr_fragment; // block address of fragment
	uint32_t	os_value2[3]; // variable sin importancia pero necesito el relleno
}__attribute__((packed));

struct ext2_dir_entry
{
	uint32_t	inode;
	uint16_t	entry_size;
	uint8_t		name_len;
	uint8_t		type;
	char		name[256];
}__attribute__((packed));
// 8 bytes + name_len

void	ext2_mount_device(struct vfs_node *node, uint32_t nb_partition);
struct ext2_inode *get_inode(struct vfs_node *node);

#endif
