#include <utils.h>
#include "fs/vfs/vfs.h"
#include "fs/ext2/ext2.h"

void	cat(char *path)
{
	struct vfs_node *node = get_vfs_node_path(path);
	if (node == 0x0)
		return ;
	if (node->type != VFS_FILE)
	{
		kprintf("Error: Not a file\n");
		kfree(node);
		return ;
	}
	char *res = node->ops->read(node);
	if (res != NULL)
	{
		kprintf("%s", res);
		kfree(res);
		kfree(node);
		return ;
	}
	kprintf("Unknown error :(\n");
	kfree(node);
}

void cd(char *path)
{
	struct vfs_node *node;
	char new_pwd[256];
	kmemset(new_pwd, 0, 256);
	kstrcpy(new_pwd, get_current_pwd());
	if (kstrncmp(path, "..", 2) == 0 || kstrncmp(path, "../", 3) == 0)
	{
		node = get_current_node();
		if (node->father == NULL)
			return ;
		set_new_node(node->father);
		if (new_pwd[kstrlen(new_pwd) - 1] == '/')
			new_pwd[kstrlen(new_pwd) - 1] = '\0';
		size_t cut = kstrrchr(new_pwd, '/');
		if (cut == 0)
			kstrcpy(new_pwd, "/");
		else
			new_pwd[cut] = '\0';
		set_new_pwd(new_pwd);
		return ;
	}
	node = get_vfs_node_path(path);
	if (node == 0x0)
		return ;
	if (node->type != VFS_DIRECTORY)
	{
		kprintf("Error: Not a directory\n");
		kfree(node);
		return ;
	}
	if (path[0] == '/')
		set_new_pwd(path);
	else
	{
		if (new_pwd[kstrlen(new_pwd) - 1] != '/')
			kstrcat(new_pwd, "/");
		kstrcat(new_pwd, path);
		set_new_pwd(new_pwd);
	}
	set_new_node(node);
}

void pwd()
{
	char *pwd = get_current_pwd();
	kprintf("%s\n", pwd);
}

// TODO in kfs-x: make this make sense with uid, id permision with current_process
bool login(char *user_buffer, char *passwd_buffer)
{
	struct vfs_node *node = get_vfs_node_path("/etc/passwd");
	if (node == 0x0)
		return (kprintf("Error: cat not read /etc/passwd :(\n"), false);
	char *user = node->ops->read(node);
	char **lines = ksplit(user, '\n');
	if (!lines)
		return (kprintf("Error: malloc failed :(\n"), kfree(node), false);

	for (int i = 0; lines[i] != NULL; i++)
	{
		char buff[128] = {0};
		size_t user_len = kstrchr(lines[i], ':');
		kstrlcpy(buff, lines[i], user_len + 1);
		char *passwd = lines[i] + user_len + 1;

		if ((kstrcmp(user_buffer, buff) == 0) && (kstrcmp(passwd_buffer, passwd) == 0))
			return (double_free(lines), kfree(node), true);
	}
	double_free(lines);
	kfree(node);
	return (false);
}

void	mount(char *path, uint32_t nb_partition)
{
	if (nb_partition <= 0 || nb_partition >= 4)
	{
		kprintf("Error: number partition not posbile :(\n");
		return ;
	}
	struct vfs_node *node = get_vfs_node_path(path);
	if (node == 0x0)
		return ;
	if (node->type != VFS_DIRECTORY || node->type == VFS_MOUNTPOINT)
	{
		kprintf("Error: Not a directory or it's already a mountpoint!\n");
		return ;
	}
	node->type = VFS_MOUNTPOINT;
	ext2_mount_device(node, nb_partition);
}

