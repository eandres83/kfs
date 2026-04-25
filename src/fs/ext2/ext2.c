#include "ext2.h"

struct vfs_node *ext2_finddir(struct vfs_node *dir_node, char *name);
void	ext2_readdir(struct vfs_node *dir_node);
char	*ext2_read(struct vfs_node *dir_node);

static struct ops ext2_ops = {
	.read = ext2_read,
	.write = NULL,
	.open = NULL,
	.close = NULL,
	.readdir = ext2_readdir,
	.finddir = ext2_finddir,
};

struct ext2_fs_info
{
	struct ext2_sb		*sb; // superblock
	struct ext2_block	*bgdt; // pointer to descriptor table
	uint32_t		size_block;
	uint32_t		sectores_per_block;
	uint32_t		bgdt_block;
};

static char *ext2_read_block(uint32_t nb_block, struct ext2_fs_info *fs_info)
{
	uint8_t *buffer = kmalloc(fs_info->size_block);
	if (!buffer)
		return (NULL);
	uint32_t lba = nb_block * fs_info->sectores_per_block;
	for (int i = 0; i < (int)fs_info->sectores_per_block; i++)
		ide_read_sector(lba + i, &buffer[512 * i]);
	return ((char*)buffer);
}

static struct ext2_inode *read_inode(struct ext2_fs_info *fs_info, uint32_t num_inodo)
{
	uint32_t offset = (num_inodo - 1) * fs_info->sb->inode_size;
	uint32_t size = offset / fs_info->size_block;

	uint32_t lba = fs_info->bgdt->addr_inode_table + size;

	char *buffer = ext2_read_block(lba, fs_info);
	if (!buffer)
		return (NULL);

	uint32_t internal_offset = offset % fs_info->size_block;
	struct ext2_inode *temp_inode = (struct ext2_inode*)(buffer + internal_offset);
	struct ext2_inode *safe_inode = kmalloc(sizeof(struct ext2_inode));
	if (!safe_inode)
		return (NULL);
	kmemcpy(safe_inode, temp_inode, sizeof(struct ext2_inode));
	kfree(buffer);
	return (safe_inode);
}

static	struct vfs_node *create_node(struct vfs_node *dir_node, struct ext2_dir_entry *dir_entry)
{
	struct vfs_node *new_node = kmalloc(sizeof(struct vfs_node));
	if (!new_node)
		return (NULL);

	kmemcpy(new_node->name, dir_entry->name, dir_entry->name_len);

	struct ext2_fs_info *fs_info = (struct ext2_fs_info*)dir_node->fs_info;
	struct ext2_inode *child_inode = read_inode(fs_info, dir_entry->inode);

	if ((child_inode->type_permisi & 0xF000) == 0x4000)
		new_node->type = VFS_DIRECTORY;
	else if ((child_inode->type_permisi & 0xF000) == 0x8000)
		new_node->type = VFS_FILE;
	else
	{
		if (dir_entry->type == 2)
			new_node->type = VFS_DIRECTORY;
		else if (dir_entry->type == 1)
			new_node->type = VFS_FILE;
		else
			new_node->type = VFS_UNKNOWN;
	}
	kfree(child_inode);

	new_node->ops = dir_node->ops;
	new_node->inode = dir_entry->inode;
	new_node->father = dir_node;
	new_node->fs_info = dir_node->fs_info;

	if (dir_node->children == NULL)
		dir_node->children = new_node;
	else
	{
		struct vfs_node *current_chil = dir_node->children;
		while (current_chil->next_to_kin != NULL)
			current_chil = current_chil->next_to_kin;
		current_chil->next_to_kin = new_node;
	}
	return (new_node);
}

char *ext2_read(struct vfs_node *dir_node)
{
	struct ext2_fs_info *fs_info = (struct ext2_fs_info*)dir_node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, dir_node->inode);

	uint32_t nb_block = inode->lower_size / fs_info->size_block;
	if ((inode->lower_size % fs_info->size_block) > 0)
		nb_block++;
	char *buffer = (char*)kmalloc(inode->lower_size + 1);
	if (!buffer)
		return (kfree(inode), NULL);
	buffer[inode->lower_size] = '\0';
	uint32_t bytes_read = 0;
	for (int i = 0; i < (int)nb_block; i++)
	{
		char *buf = ext2_read_block(inode->direct_block[i], fs_info);
		uint32_t to_read = inode->lower_size - (i * 4096);
		if (to_read >= 4096)
		{
			kmemcpy(&buffer[4096 * i], buf, 4096);
			bytes_read += 4096;
		}
		else
		{
			kmemcpy(&buffer[4096 * i], buf, to_read);
			bytes_read += to_read;
		}
		kfree(buf);
	}
	kfree(inode);
	return (buffer);
}

struct vfs_node *ext2_finddir(struct vfs_node *dir_node, char *name)
{
	struct ext2_fs_info *fs_info = (struct ext2_fs_info*)dir_node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, dir_node->inode);

	char *block = ext2_read_block(inode->direct_block[0], fs_info);
	struct ext2_dir_entry *dir_entry = (struct ext2_dir_entry*)block;

	uint32_t bytes_read = 0;
	while (bytes_read < fs_info->size_block)
	{
		if (dir_entry->inode != 0)
		{
			char buf[256] = {0};
			kmemcpy(buf, dir_entry->name, dir_entry->name_len);
			if (kstrcmp(buf, name) == 0)
			{
				struct vfs_node *res = create_node(dir_node, dir_entry);
				kfree(block);
				kfree(inode);
				return (res);
			}
		}
		if (dir_entry->entry_size == 0)
		{
			kprintf("Error en entry_size == 0\n");
			break;
		}
		bytes_read += dir_entry->entry_size;
		dir_entry = (struct ext2_dir_entry*)((char*)dir_entry + dir_entry->entry_size);
	}
	kfree(block);
	kfree(inode);
	return (NULL);
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
			create_node(dir_node, dir_entry);

		if (dir_entry->entry_size == 0)
		{
			kprintf("Error en entry_size == 0\n");
			break;
		}
		bytes_read += dir_entry->entry_size;
		dir_entry = (struct ext2_dir_entry*)((char*)dir_entry + dir_entry->entry_size);
	}
	kfree(inode);
	kfree(block);
}

void init_ext2()
{
	struct ext2_fs_info *fs_info = kmalloc(sizeof(struct ext2_fs_info));
	if (!fs_info)
		return ;

	kmemset(fs_info, 0, sizeof(struct ext2_fs_info));
	fs_info->sb = kmalloc(sizeof(struct ext2_sb));
	if (!fs_info->sb)
		return ;
	// calcular superblock
	uint8_t	buffer[1024];
	ide_read_sector(2, buffer);
	ide_read_sector(3, &buffer[512]);

	kmemcpy(fs_info->sb, buffer, sizeof(struct ext2_sb));
	if (fs_info->sb->magic_nb == 0xef53)
		kprintf("El numero -> %x\n", fs_info->sb->magic_nb);

	if (fs_info->sb->version_major == 0)
		fs_info->sb->inode_size = 128;

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

	// montar ext2 :)
	vfs->ops = &ext2_ops;
	vfs->fs_info = fs_info;
	vfs->inode = 2;

	kfree(buffer2);
}

