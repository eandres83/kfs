#include <sys/types.h>
#include "task/task.h"
#include "kernel/sys.h"

uid_t setuid(uint32_t new_uid)
{
	proc_t	*process = get_current_process();
	if (new_uid > 65535)
		new_uid = 65534;
	if (process->euid == 0)
	{
		process->euid = new_uid;
		process->ruid = new_uid;
		return (0);
	}
	else if (process->ruid == new_uid)
	{
		process->euid = new_uid;
		return (0);
	}
	return (-1);
}

uid_t getuid()
{
	proc_t	*process = get_current_process();
	uint32_t uid = process->euid;
	if (uid > 65535)
		uid = 65534;
	return (uid);
}

gid_t setgid(uint32_t gid)
{
	proc_t	*process = get_current_process();
	if (gid > 65535)
		gid = 65534;
	process->rgid = gid;
	return (0);
}

gid_t	getgid()
{
	proc_t	*process = get_current_process();
	uint32_t gid = process->rgid;
	if (gid > 65535)
		gid = 65534;
	return (gid);
}

uid_t	geteuid()
{
	proc_t	*process = get_current_process();
	uint32_t uid = process->euid;
	if (uid > 65535)
		uid = 65534;
	return (uid);
}

gid_t	getegid()
{
	proc_t	*process = get_current_process();
	uint32_t gid = process->egid;
	if (gid > 65535)
		gid = 65534;
	return (gid);
}

uid_t	getuid32()
{
	proc_t	*process = get_current_process();
	return (process->euid);
}

gid_t	getgid32()
{
	proc_t	*process = get_current_process();
	return (process->rgid);
}

uid_t	setuid32(uint32_t uid)
{
	proc_t	*process = get_current_process();
	if (process->euid == 0)
	{
		process->euid = uid;
		process->ruid = uid;
		return (0);
	}
	else if (process->ruid == uid)
	{
		process->euid = uid;
		return (0);
	}
	return (-1);
}

gid_t	setgid32(uint32_t gid)
{
	proc_t	*process = get_current_process();
	process->rgid = gid;
	return (0);
}

uid_t	geteuid32()
{
	proc_t	*process = get_current_process();
	return (process->euid);
}

gid_t	getegid32()
{
	proc_t	*process = get_current_process();
	return (process->egid);
}

static char *pwd_right(char *path)
{
	char new_pwd[256];
	kmemset(new_pwd, 0, 256);
	if (path[0] != '/')
	{
		// ruta relativa
		if (getcwd(new_pwd, 256) == NULL)
			return (NULL);
		if (new_pwd[kstrlen(new_pwd) - 1] != '/')
			kstrcat(new_pwd, "/");
		kstrcat(new_pwd, path);
	}
	else
		kstrcpy(new_pwd, path);
	char **array = ksplit(new_pwd, '/');
	if (!array)
		return (NULL);

	char *buff[256];
	int top = 0;
	for (int i = 0; array[i] != NULL; i++)
	{
		if (kstrcmp(array[i], ".") == 0 || kstrlen(array[i]) == 0)
			continue;
		else if (kstrcmp(array[i], "..") == 0)
		{
			if (top > 0)
				top--;
		}
		else
		{
			buff[top] = array[i];
			top++;
		}
	}
	double_free(array);
	char *pwd = (char*)kmalloc(sizeof(char) * 256);
	if (!pwd)
		return (NULL);
	kmemset(pwd, 0, 256);
	kstrcpy(pwd, "/");
	for (int i = 0; i < top; i++)
	{
		kstrcat(pwd, buff[i]);
		if (i < top -1)
			kstrcat(pwd, "/");
	}
	return (pwd);
}

ssize_t	chdir(char *path)
{
	if (!path)
		return (-1);
	char *new_pwd = pwd_right(path);
	if (new_pwd == NULL)
		return (-1);

	struct vfs_node *node = get_vfs_node_path(new_pwd);
	if (node == 0x0)
		return (kfree(new_pwd), -1);
	if (node->type != VFS_DIRECTORY && node->type != VFS_MOUNTPOINT)
	{
		kdebug("Error: Not a directory\n");
		return (kfree(new_pwd), -1);
	}
	set_new_node(node);
	return (0);
}

char	*getcwd(char *buf, size_t size)
{
	proc_t *current_process = get_current_process();
	struct vfs_node *node = current_process->node;
	kmemset(buf, 0, size);
	if (node->father == NULL)
	{
		buf[0] = '/';
		return (buf);
	}
	// tmp pointer
	char *p = buf + size -1;
	*p = '\0';
	while (node->father != NULL)
	{
		int len  = kstrlen(node->name);
		if (p - buf < len + 1)
			return (NULL);
		p -= len;
		kmemcpy(p, node->name, len);
		p--;
		*p = '/';
		node = node->father;
	}
	int total_len = kstrlen(p);
	kmemcpy(buf, p, total_len + 1);
	return (buf);
}

