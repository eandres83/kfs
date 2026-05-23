#include "ext2.h"

struct vfs_node *ext2_finddir(struct vfs_node *dir_node, char *name);
void	ext2_readdir(struct vfs_node *dir_node);
char	*ext2_read(struct vfs_node *dir_node);
size_t 	ext2_write(struct vfs_node *node, char *str, size_t len);
ssize_t ext2_open(struct vfs_node *node);
size_t	ext2_close(struct vfs_node *node);

static struct ops ext2_ops = {
	.read = ext2_read,
	.write = ext2_write,
	.open = ext2_open,
	.close = ext2_close,
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
	uint32_t		lba_offset; // offset if have partition
};

static void ext2_write_block(uint32_t nb_block, struct ext2_fs_info *fs_info, char *str)
{
	uint32_t lba = nb_block * fs_info->sectores_per_block;
	lba += fs_info->lba_offset;
	for (int i = 0; i < (int)fs_info->sectores_per_block; i++)
		ide_write_sector(lba + i, (uint8_t*)&str[512 * i]);
}

static char *ext2_read_block(uint32_t nb_block, struct ext2_fs_info *fs_info)
{
	uint8_t *buffer = kmalloc(fs_info->size_block);
	if (!buffer)
		return (NULL);
	uint32_t lba = nb_block * fs_info->sectores_per_block;
	lba += fs_info->lba_offset;
	for (int i = 0; i < (int)fs_info->sectores_per_block; i++)
		ide_read_sector(lba + i, &buffer[512 * i]);
	return ((char*)buffer);
}

uint32_t find_block(struct ext2_fs_info *fs_info, uint32_t addr_block)
{
	uint32_t indx;
	char *bitmap = ext2_read_block(addr_block, fs_info);
	for (int i = 0; i < (int)fs_info->size_block; i++)
	{
		if ((unsigned char)bitmap[i] == 0xFF)
			continue;
		for (int j = 0; j < 8; j++)
		{
			if ((bitmap[i] & (1 << j)) == 0)
			{
				bitmap[i] |= (1 << j);
				indx = (i * 8) + j;
				ext2_write_block(addr_block, fs_info, bitmap);
				return (kfree(bitmap), indx);
			}
		}
	}
	kfree(bitmap);
	kprintf("Esto no se deberia de ver :(\n");
	return (0);
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

struct ext2_inode *get_inode(struct vfs_node *node)
{
	struct ext2_fs_info *fs_info = (struct ext2_fs_info *)node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, node->inode);
	if (!inode)
		return (NULL);
	return (inode);
}

ssize_t ext2_open(struct vfs_node *node)
{
	if (!node || !node->fs_info)
		return (-1);

	struct ext2_fs_info *fs_info = (struct ext2_fs_info *)node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, node->inode);
	if (!inode)
		return (-2);

	node->size = inode->lower_size;
	kfree(inode);
	return (0);
}

size_t	ext2_close(struct vfs_node *node)
{
	if (!node)
		return (-1);
	return (0);
}

size_t ext2_write(struct vfs_node *node, char *str, size_t len)
{
	struct ext2_fs_info *fs_info = (struct ext2_fs_info *)node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, node->inode);
	if (!inode)
		return (1);

	size_t bytes_written = 0;
	while (bytes_written < len)
	{
		uint32_t current_size = inode->lower_size + bytes_written;
		uint32_t block_index = current_size / fs_info->size_block;
		uint32_t offset_in_block = current_size % fs_info->size_block;

		if (block_index >= 12)
		{
			kprintf("Error: file to large :(\n");
			break;
		}
		if (inode->direct_block[block_index] == 0)
		{
			inode->direct_block[block_index] = find_block(fs_info, fs_info->bgdt->addr_inode_bitmap);
			if (inode->direct_block[block_index] == 0)
			{
				kprintf("Error: cannot find a free block\n");
				break;
			}
		}
		char *buffer = ext2_read_block(inode->direct_block[block_index], fs_info);
		if (!buffer)
			break;
		uint32_t space_in_block = fs_info->size_block - offset_in_block;
		uint32_t chunk_size = (len - bytes_written < space_in_block);
		kmemcpy(buffer + offset_in_block, str + bytes_written, chunk_size);
		ext2_write_block(inode->direct_block[block_index], fs_info, buffer);
		kfree(buffer);
		bytes_written += chunk_size;
	}
	if (bytes_written > 0)
	{
		inode->lower_size += bytes_written;
		uint32_t inode_index = node->inode -1;
		uint32_t offset_bytes = inode_index * fs_info->sb->inode_size;
		uint32_t block_offset = offset_bytes / fs_info->size_block;
		uint32_t internal_offset = offset_bytes % fs_info->size_block;
		uint32_t table_lba = fs_info->bgdt->addr_inode_table + block_offset;
		char *table_buffer = ext2_read_block(table_lba, fs_info);
		if (table_buffer)
		{
			kmemcpy(table_buffer + internal_offset, inode, fs_info->sb->inode_size);
			ext2_write_block(table_lba, fs_info, table_buffer);
			kfree(table_buffer);
		}
		else
			kprintf("Error: cannot read inode table\n");
	}
	kfree(inode);
	return (bytes_written);
}

size_t ext2_create(struct vfs_node *node, char *str, char *file, uint32_t rights)
{
	(void)rights;
	struct ext2_fs_info *fs_info = (struct ext2_fs_info *)node->fs_info;

	// write new block
	uint32_t indx = find_block(fs_info, fs_info->bgdt->addr_block_bitmap);
	if (indx == 0)
		return (kprintf("block not found\n"), 1);
	char buf[fs_info->size_block];
	kmemset(buf, 0, fs_info->size_block);
	kstrcpy(buf, str);
	ext2_write_block(indx, fs_info, buf);

	// create new inode for new block
	uint32_t indx_inode = find_block(fs_info, fs_info->bgdt->addr_inode_bitmap);
	if (indx_inode == 0)
		return (kprintf("block not found\n"), 1);

	struct ext2_inode *new_inode = kmalloc(sizeof(struct ext2_inode));
	if (!new_inode)
		return (1);
	new_inode->direct_block[0] = indx;
	new_inode->nb_hard_links = 1;
	new_inode->nb_disk_sector = fs_info->size_block / 512;
	new_inode->type_permisi = 0x8000;
	new_inode->lower_size = (uint32_t)kstrlen(str);

	uint32_t block = fs_info->bgdt->addr_inode_table + ((indx_inode * fs_info->sb->inode_size) / fs_info->size_block);
	char *buffer = ext2_read_block(block, fs_info);

	uint32_t offset = (indx_inode * fs_info->sb->inode_size) % fs_info->size_block;
	char *addr_new_inode = buffer + offset;
	kmemcpy(addr_new_inode, new_inode, fs_info->sb->inode_size);

	ext2_write_block(block, fs_info, buffer);
	kfree(new_inode);
	kfree(buffer);

	// create dir_entry
	struct ext2_inode *inode = read_inode(fs_info, node->inode);
	if (!inode)
		return (1);

	struct ext2_dir_entry *dir_entry = NULL;
	uint32_t last_block_indx = (inode->lower_size / fs_info->size_block);
	if (inode->lower_size % fs_info->size_block == 0 && last_block_indx > 0)
		last_block_indx--; // por si es multiplo

	char *inode_block = ext2_read_block(inode->direct_block[last_block_indx], fs_info);
	dir_entry = (struct ext2_dir_entry *)inode_block;

	uint32_t bytes_read = 0;
	while (bytes_read < fs_info->size_block)
	{
		if ((bytes_read + dir_entry->entry_size) >= fs_info->size_block)
			break;
		bytes_read += dir_entry->entry_size;
		dir_entry = (struct ext2_dir_entry*)((char*)dir_entry + dir_entry->entry_size);
	}
	uint32_t real_size = 8 + kstrlen(dir_entry->name); // 8 = dir_entry struct
	real_size = ((real_size + 3) / 4) * 4; // round up

	if (fs_info->size_block - bytes_read - real_size < real_size)
		return (kprintf("Error: no left space\n"), 1);
	uint32_t original_size = dir_entry->entry_size;
	dir_entry->entry_size = real_size;

	struct ext2_dir_entry *new_dir = (struct ext2_dir_entry*)((char*)dir_entry + real_size);
	new_dir->entry_size = original_size - real_size;
	new_dir->inode = indx_inode + 1;
	new_dir->name_len = kstrlen(file);
	new_dir->type = 1;
	kmemcpy(new_dir->name, file, new_dir->name_len);

	ext2_write_block(inode->direct_block[last_block_indx], fs_info, inode_block);
	kfree(inode_block);
	kfree(inode);
	return (inode->lower_size);
}

// Doubly and Triply indirect block pointers are intentionally NOT implemented
// Total supported file size ceiling: ~268 KB
char *ext2_read(struct vfs_node *dir_node)
{
	struct ext2_fs_info *fs_info = (struct ext2_fs_info*)dir_node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, dir_node->inode);
	if (!inode)
		return (NULL);

	uint32_t nb_block = inode->lower_size / fs_info->size_block;
	if ((inode->lower_size % fs_info->size_block) > 0)
		nb_block++;
	char *buffer = (char*)kmalloc(inode->lower_size + 1);
	if (!buffer)
		return (kfree(inode), NULL);
	buffer[inode->lower_size] = '\0';
	uint32_t bytes_read = 0;
	uint32_t *indirect_block_cache = NULL;
	uint32_t pointer_per_block = fs_info->size_block / 4;
	for (uint32_t i = 0; i < nb_block; i++)
	{
		char *buf = NULL;
		if (i < 12)
			buf = ext2_read_block(inode->direct_block[i], fs_info);
		else if (i >= 12 && i < (12 + pointer_per_block))
		{
			if (indirect_block_cache == NULL)
			{
				indirect_block_cache = (uint32_t*)ext2_read_block(inode->singly_indirect_block, fs_info);
				if (!indirect_block_cache)
					return (kfree(buffer), kfree(inode), NULL);
			}
			uint32_t real_data_lba = indirect_block_cache[i - 12];
			buf = ext2_read_block(real_data_lba, fs_info);
		}
		if (!buf)
		{
			if (indirect_block_cache)
				kfree(indirect_block_cache);
			return (kfree(buffer), kfree(inode), NULL);
		}
		uint32_t to_read = inode->lower_size - (i * fs_info->size_block);
		if (to_read >= fs_info->size_block)
		{
			kmemcpy(&buffer[fs_info->size_block * i], buf, fs_info->size_block);
			bytes_read += fs_info->size_block;
		}
		else
		{
			kmemcpy(&buffer[fs_info->size_block * i], buf, to_read);
			bytes_read += to_read;
		}
		kfree(buf);
	}
	if (indirect_block_cache)
		kfree(indirect_block_cache);
	kfree(inode);
	return (buffer);
}

static	struct vfs_node *create_node(struct vfs_node *dir_node, struct ext2_dir_entry *dir_entry)
{
	struct vfs_node *new_node = kmalloc(sizeof(struct vfs_node));
	if (!new_node)
		return (NULL);
	kmemset(new_node, 0, sizeof(struct vfs_node));

	kmemcpy(new_node->name, dir_entry->name, dir_entry->name_len);
	new_node->name[dir_entry->name_len] = '\0';

	struct ext2_fs_info *fs_info = (struct ext2_fs_info*)dir_node->fs_info;
	struct ext2_inode *child_inode = read_inode(fs_info, dir_entry->inode);
	if (!child_inode)
		return (NULL);

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

struct vfs_node *ext2_finddir(struct vfs_node *dir_node, char *name)
{
	struct ext2_fs_info *fs_info = (struct ext2_fs_info*)dir_node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, dir_node->inode);
	if (!inode)
		return (NULL);

	uint32_t nb_block = inode->lower_size / fs_info->size_block;
	for (int i = 0; i < (int)nb_block; i++)
	{
		char *block = ext2_read_block(inode->direct_block[i], fs_info);
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
				kprintf("Errro: finddir entry size == 0\n");
				break;
			}
			bytes_read += dir_entry->entry_size;
			dir_entry = (struct ext2_dir_entry*)((char*)dir_entry + dir_entry->entry_size);
		}
		kfree(block);
	}
	kfree(inode);
	return (NULL);
}

void ext2_readdir(struct vfs_node *dir_node)
{
	struct ext2_fs_info *fs_info = (struct ext2_fs_info*)dir_node->fs_info;
	struct ext2_inode *inode = read_inode(fs_info, dir_node->inode);
	if (!inode)
		return ;

	uint32_t nb_block = inode->lower_size / fs_info->size_block;
	for (int i = 0; i < (int)nb_block; i++)
	{
		char *block = ext2_read_block(inode->direct_block[i], fs_info);

		struct ext2_dir_entry *dir_entry = (struct ext2_dir_entry *)block;

		uint32_t bytes_read = 0;
		while (bytes_read < fs_info->size_block)
		{
			if (dir_entry->inode != 0)
			{
				struct vfs_node *child = dir_node->children;
				bool found = false;
				while (child != NULL)
				{
					if (kstrncmp(child->name, dir_entry->name, dir_entry->name_len) == 0 && child->name[dir_entry->name_len] == '\0')
					{
						found = true;
						break;
					}
					child = child->next_to_kin;
				}
				if (!found)
					create_node(dir_node, dir_entry);
			}
			if (dir_entry->entry_size == 0)
			{
				kprintf("Error: readdir entry size == 0\n");
				break;
			}
			bytes_read += dir_entry->entry_size;
			dir_entry = (struct ext2_dir_entry*)((char*)dir_entry + dir_entry->entry_size);
		}
		kfree(block);
	}
	kfree(inode);
}

static struct ext2_fs_info *init_ext2(uint32_t lba_offset)
{
	struct ext2_fs_info *fs_info = kmalloc(sizeof(struct ext2_fs_info));
	if (!fs_info)
		return (NULL);

	kmemset(fs_info, 0, sizeof(struct ext2_fs_info));
	fs_info->lba_offset = lba_offset;
	fs_info->sb = kmalloc(sizeof(struct ext2_sb));
	if (!fs_info->sb)
		return (NULL);
	// calcular superblock
	uint8_t	buffer[1024];
	ide_read_sector(lba_offset + 2, buffer);
	ide_read_sector(lba_offset + 3, &buffer[512]);

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
		return (NULL);
	kmemcpy(fs_info->bgdt, buffer2, sizeof(struct ext2_block));

	kfree(buffer2);
	return (fs_info);
}

void	ext2_mount_device(struct vfs_node *node, uint32_t nb_partition)
{
	struct mbr_entry *mbr = partition(nb_partition);
	if (!mbr)
		return ;
	struct ext2_fs_info *fs_info = init_ext2(mbr->lba_start);
	if (!fs_info)
		return ;

	node->ops = &ext2_ops;
	node->fs_info = fs_info;
	node->inode = 2;
}

