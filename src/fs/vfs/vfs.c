#include "vfs.h"
#include "fs/ext2/ext2.h"

struct vfs_node *vfs = NULL;

// /home/eandres/Documents/kfs/src/fs/vfs/vfs.c
struct vfs_node *get_vfs_node_path(char *path)
{
	char **word = ksplit(path, '/');
	if (!word)
		return (NULL);

	struct vfs_node *current_node = vfs;
	for (int i = 0; word[i] != NULL; i++)
	{
		if (kstrlen(word[i]) <= 0)
			continue;
		current_node = current_node->ops->finddir(current_node, word[i]);
		if (current_node == 0x0)
			return (kprintf("Error: No such file or directory\n"), double_free(word), NULL);
		if (current_node->type != VFS_FILE && word[i + 1] == NULL)
			return (kprintf("Error: %s: Is a directory\n", path), double_free(word), NULL);
//		kprintf("Buscando '%s' dentro de nodo actual (tipo: %d)\n", word[i], current_node->type);
	}
	double_free(word);
	return (current_node);
}

void init_vfs()
{
	vfs = kmalloc(sizeof(struct vfs_node));
	if (!vfs)
		return ;

	kmemset(vfs, 0, sizeof(struct vfs_node));
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

