#include <utils.h>
#include "fs/vfs/vfs.h"

void	cat(char *path)
{
	struct vfs_node *node = get_vfs_node_path(path);
	if (node == 0x0)
		return ;
	if (node->type != VFS_FILE)
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

bool login(char *user_buffer, char *passwd_buffer)
{
	struct vfs_node *node = get_vfs_node_path("/etc/passwd");
	if (node == 0x0)
		return (kprintf("Error: cat not read /etc/passwd :(\n"), false);
	char *user = node->ops->read(node);
	char **lines = ksplit(user, '\n');
	if (!lines)
		return (kprintf("Error: malloc failed :(\n"), false);

	for (int i = 0; lines[i] != NULL; i++)
	{
		char buff[128] = {0};
		size_t user_len = kstrchr(lines[i], ':');
		kstrlcpy(buff, lines[i], user_len + 1);
		char *passwd = lines[i] + user_len + 1;

		if ((kstrcmp(user_buffer, buff) == 0) && (kstrcmp(passwd_buffer, passwd) == 0))
			return (true);
	}
	double_free(lines);
	return (false);
}

