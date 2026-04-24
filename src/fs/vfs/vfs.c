#include "vfs.h"
#include "fs/ext2/ext2.h"

struct vfs_node *vfs = NULL;

// /home/eandres/Documents/kfs/src/fs/vfs/vfs.c
struct vfs_node *get_vfs_node_path(char *path)
{
	kprintf("Llamando a get_vfs_node_path para conseguir el split\n");
	char **word = ksplit(path, '/');
	if (!word)
		return (NULL);

	struct vfs_node *current_node = vfs;
	for (int i = 0; word[i] != NULL; i++)
	{
		if (kstrlen(word[i]) <= 0)
			continue;
		if (current_node->type != VFS_DIRECTORY && word[i + 1] != NULL)
			return (kprintf("Error: Not a directory\n"), double_free(word), NULL);
		current_node = current_node->ops->finddir(vfs, word[i]);
		if (current_node == NULL)
			return (kprintf("Error: No such file or directory\n"), double_free(word), NULL);
	}
	double_free(word);
	kprintf("Saliendo de get_vfs_node_path\n");
	return (current_node);
}

void init_vfs()
{
	vfs = kmalloc(sizeof(struct vfs_node));
	if (!vfs)
		return ;

	vfs->name[0] = '/';
	vfs->size = 0;
	vfs->inode = 0;
	vfs->links = 0;
	vfs->rights = 0;
	vfs->fs_info = NULL;
	vfs->type = VFS_DIRECTORY;
	vfs->ops = NULL;
	vfs->father = NULL;
	vfs->children = NULL;
	vfs->next_to_kin = NULL;
	vfs->master = NULL;
}

