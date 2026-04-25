#include <utils.h>
#include "fs/vfs/vfs.h"

void	cat(char *path)
{
	struct vfs_node *node = get_vfs_node_path(path);
	if (node != NULL && node->type != VFS_FILE)
	{
		kprintf("Error: Not a file\n");
		return ;
	}
	char *res = node->ops->read(node);
	if (res != NULL)
	{
		kprintf("%s", res);
		kfree(res);
		return ;
	}
	kprintf("Unknown error :(\n");
}

void cd(char *path)
{
	struct vfs_node *node = get_vfs_node_path(path);
	if (node == NULL || node->type != VFS_DIRECTORY)
	{
		kprintf("Error: Not a directory\n");
		return ;
	}
	
}

void pwd()
{
	char *pwd = get_current_pwd();
	kprintf("%s", pwd);
}

