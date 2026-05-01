#include "fs/dummy.h"

char	*dummy_read(struct vfs_node *node);

static struct ops dummy_ops = {
	.read = dummy_read,
	.write = NULL,
	.open = NULL,
	.close = NULL,
	.readdir = NULL,
	.finddir = NULL,
};

char	*dummy_read(struct vfs_node *node)
{
	(void)node;
	char *res = "hola\n";
	return (res);
}

void	mount_dummy(struct vfs_node *node)
{
	node->ops = &dummy_ops;
	node->fs_info = NULL;
	node->inode = 2;
}

