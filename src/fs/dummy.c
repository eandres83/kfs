#include "fs/dummy.h"

char	*dummy_read(struct vfs_node *node);
size_t	dummy_write(struct vfs_node *node, char *str, char *file_name);

static struct ops dummy_ops = {
	.read = dummy_read,
	.write = dummy_write,
	.open = NULL,
	.close = NULL,
	.readdir = NULL,
	.finddir = NULL,
};

size_t	dummy_write(struct vfs_node *node, char *str, char *file_name)
{
	(void)node;
	(void)str;
	(void)file_name;

	kprintf("que putas pasa aqui bro\n");
	return (0);
}

char	*dummy_read(struct vfs_node *node)
{
	(void)node;
	char *res = "HOLA desde el puto dummy_read\n";
	return (res);
}

void	mount_dummy(struct vfs_node *node)
{
	struct vfs_node *backup = kmalloc(sizeof(struct vfs_node));
	if (!backup)
		return ;

	kmemcpy(backup, node, sizeof(struct vfs_node));
	if (node->type != VFS_DIRECTORY || node->type == VFS_MOUNTPOINT)
	{
		kprintf("Error: already exist or it's a directory\n");
		return ;
	}
	node->master = backup;
	node->type = VFS_MOUNTPOINT;
	node->ops = &dummy_ops;
	node->fs_info = NULL;
	node->inode = 2;
}

