#include "uaccess.h"
#include "fs/ext2/ext2.h"
#include "task/task.h"
#include "fs/fd.h"

static	int check_permission(uint16_t permission, uint32_t offset, int mode)
{
	if (mode & X_OK)
	{
		if ((permission & (1 << offset)) == 0)
			return (-1);
	}
	if (mode & W_OK)
	{
		if ((permission & (1 << (offset + 1))) == 0)
			return (-1);
	}
	if (mode & R_OK)
	{
		if ((permission & (1 << (offset + 2))) == 0)
			return (-1);
	}
	return (0);
}

static int check_node_permision(struct vfs_node *node, int mode)
{
	proc_t *current_process = get_current_process();
	int ret = -1;

	if (current_process->euid == 0)
		ret = 0;
	else if (current_process->euid == node->uid)
		ret = check_permission(node->rights, 6, mode);
	else if (current_process->egid == node->gid)
		ret = check_permission(node->rights, 3, mode);
	else
		ret = check_permission(node->rights, 0, mode);
	return (ret);
}

ssize_t access(char *filename, int mode)
{
	char *kbuff = (char*)kmalloc(kstrlen(filename) + 1);
	if (!kbuff)
		return (-1);
	kmemset(kbuff, 0, kstrlen(filename) + 1);
	copy_from_user_wrap(kbuff, filename, kstrlen(filename));
	struct vfs_node *node = get_vfs_node_path(kbuff);
	if (!node)
		return (-1);
	int ret = check_node_permision(node, mode);
	return (ret);
}

ssize_t	open(char *filename, uint32_t flags, uint32_t mode)
{
	(void)mode;
	proc_t *current = get_current_process();

	int fd = fd_allocate(current);
	if (fd < 0)
		return (-24);

	char *kbuff = (char*)kmalloc(kstrlen(filename) + 1);
	if (!kbuff)
		return (-1);
	kmemset(kbuff, 0, kstrlen(filename) + 1);
	ssize_t res = copy_from_user_wrap(kbuff, filename, kstrlen(filename));
	if (res == -1)
		return (-1);
	struct vfs_node *node = get_vfs_node_path(kbuff);
	if (!node)
	{
		fd_free(current, fd);
		return (-2);
	}
	uint32_t intent = flags & 3;
	int access_mode = 0;

	if (intent == O_RDONLY)
		access_mode = R_OK;
	else if (intent == O_WRONLY)
		access_mode = W_OK;
	else if (intent == O_RDWR)
		access_mode = (R_OK | W_OK);
	if (check_node_permision(node, access_mode) == -1)
		return (fd_free(current, fd), -13);
	if (node->ops && node->ops->open)
	{
		int err = node->ops->open(node);
		if (err < 0)
		{
			fd_free(current, fd);
			return (err);
		}
	}
	struct file *new_file = kmalloc(sizeof(struct file));
	if (!new_file)
	{
		fd_free(current, fd);
		return (-12);
	}
	new_file->node = node;
	new_file->flags = flags;
	new_file->offset = 0;
	new_file->ref_count = 1;

	current->fd_table[fd] = new_file;
	return (fd);
}

ssize_t	close(int fd)
{
	if (fd < 0 || fd >= MAX_FD)
		return (-1);
	proc_t *current = get_current_process();

	struct file *f = current->fd_table[fd];
	if (!f)
		return (-11);
	fd_free(current, fd);
	return (0);
}

ssize_t read(int fd, void *buf, size_t count)
{
	struct file *filed = fd_get(get_current_process(), fd);
	if (filed == NULL || filed->node == NULL || filed->node->ops->read == NULL)
		return (-1);
	if (filed->flags & O_WRONLY)
		return (-1);
	char *kbuff = (char*)kmalloc(sizeof(char) * count);
	if (!kbuff)
		return (-1);
	kmemset(kbuff, 0, count);
	ssize_t res = filed->node->ops->read(filed->node, kbuff, count, filed->offset);
	if (res < 0)
		return (kfree(kbuff), -1);
	else if (res == 0)
		return (kfree(kbuff), 0);
	ssize_t user_read = copy_to_user_wrap((char*)buf, kbuff, res);
	if (user_read < 0)
		return (kfree(kbuff), -1);

	filed->offset += res;
	kfree(kbuff);
	return (res);
}

ssize_t	write(int fd, const char *str, size_t count)
{
	struct file *filed = fd_get(get_current_process(), fd);
	if (filed == NULL || filed->node == NULL || filed->node->ops->write == NULL)
		return (-1);
	if (filed->flags & O_RDONLY)
		return (-1);

	char *kbuff = (char*)kmalloc(sizeof(char) * count);
	if (!kbuff)
		return (-1);
	kmemset(kbuff, 0, count);
	ssize_t user_read = copy_from_user_wrap(kbuff, str, count);
	if (user_read < 0)
		return (kfree(kbuff), -1);
	ssize_t res = filed->node->ops->write(filed->node, kbuff, count, filed->offset);
	if (res < 0)
		return (kfree(kbuff), -1);

	filed->offset += res;
	kfree(kbuff);
	return (res);
}

