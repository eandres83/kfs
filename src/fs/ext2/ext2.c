#include "ext2.h"

struct ext2_fs_info
{
	struct ext2_sb		*sb; // superblock
	struct ext2_block	*bgdt; // pointer to descriptor table
	uint32_t		size_block;
	uint32_t		sectores_per_block;
	uint32_t		bgdt_block;
};

struct ext2_inode *read_inode(struct ext2_fs_info *fs_info, uint32_t num_inodo)
{
	uint32_t lba = fs_info->bgdt->addr_inode_table * fs_info->sectores_per_block;

	char *buffer = ext2_read_block(lba, fs_info);
	if (!buffer)
		return (NULL);

	uint32_t offset = (num_inodo - 1) * fs_info->sb->inode_size;

	struct ext2_inode *inode_main = (struct ext2_inode*)(buffer + fs_info->sb->inode_size);
	free(buffer);
	return (inode_main);
}

char *ext2_read_block(uint32_t nb_block, struct ext2_fs_info *fs_info)
{
	uint8_t *buffer = kmalloc(fs_info->size_block);
	if (!buffer)
		return (NULL);
	uint32_t lba = nb_block * fs_info->sectores_per_block;
	for (int i = 0; i < (int)fs_info->sectores_per_block; i++)
		ide_read_sector(lba + i, &buffer[512 * i]);
	return ((char*)buffer);
}

void ext2_readdir(struct vfs_node *dir_node)
{
	struct ext2_fs_info *fs_info = (struct ext2_fs_info*)dir_node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, dir_node->inode);

	char *block = ext2_read_block(inode->direct_block[0], fs_info);

	struct ext2_dir_entry *dir_entry = (struct ext2_dir_entry *)block;

	uint32_t bytes_read = 0;
	while (bytes_read < fs_info->size_block)
	{
		if (dir_entry->inode != 0)
		{
			char buf[256] = {0};
			kmemcpy(buf, dir_entry->name, dir_entry->name_len);
			kprintf("%s ", buf);
		}
		bytes_read += dir_entry->entry_size;
		dir_entry = (struct ext2_dir_entry*)((char*)dir_entry + dir_entry->entry_size);
	}
	free(block);
}

void init_ext2()
{
	struct ext2_fs_info *fs_info = kmalloc(sizeof(struct ext2_fs_info));
	if (!fs_info)
		return ;

	fs_info->sb = kmalloc(sizeof(struct ext2_sb));
	if (!fs_info->sb)
		return ;
	// calcular superblock
	uint8_t	buffer[1024];

	ide_read_sector(2, buffer);
	ide_read_sector(3, &buffer[512]);

	kmemcpy(fs_info->sb, buffer, sizeof(struct ext2_sb));
	if (fs_info->sb->magic_nb == 0xef53)
		kprintf("El numero -> %d\n", fs_info->sb->magic_nb);

	// primero calcular la posicion del lba dependiendo del tamano de los bloques
	// que depende del tamano del hard disk
	fs_info->size_block = 1024 << fs_info->sb->block_size;
	fs_info->sectores_per_block = fs_info->size_block / 512;

	if (fs_info->size_block == 1024)
		fs_info->bgdt_block = 2;
	else
		fs_info->bgdt_block = 1;

	// calcular BGDT -> block group descriptor table
	char *buffer2 = ext2_read_block(fs_info->bgdt_block, fs_info);

	fs_info->bgdt = kmalloc(sizeof(struct ext2_block));
	if (!fs_info->bgdt)
		return ;
	kmemcpy(fs_info->bgdt, buffer2, sizeof(struct ext2_block));

	kfree(buffer2);
}

