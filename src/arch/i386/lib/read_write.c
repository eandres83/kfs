#include "arch/i386/lib/uaccess.h"
#include "fs/ext2/ext2.h"
#include "task/task.h"

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
	struct ext2_inode *inode = get_inode(node);
	if (!inode)
		return (-1);
	proc_t *current_process = get_current_process();
	int ret = -1;

	if (current_process->euid == 0)
		ret = 0;
	else if (current_process->euid == inode->user_id)
		ret = check_permission(inode->type_permisi, 6, mode);
	else if (current_process->gid == inode->group_id)
		ret = check_permission(inode->type_permisi, 3, mode);
	else
		ret = check_permission(inode->type_permisi, 0, mode);
	kfree(inode);
	return (ret);
}

ssize_t access(char *filename, int mode)
{
	struct vfs_node *node = get_vfs_node_path(filename);
	if (node == 0x0)
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

	struct vfs_node *node = get_vfs_node_path(filename);
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
	new_file->indx = 0;
	new_file->ref_count = 1;

	current->fd_table[fd] = new_file;
	return (fd);
}

ssize_t	close(int fd)
{
	if (fd >= 2)
		return (-1);
	proc_t *current = get_current_process();

	struct file *f = fd_get(current, fd);
	if (!f)
		return (-9);
	f->ref_count--;

	if (f->ref_count == 0)
	{
		if (f->node && f->node->ops && f->node->ops->close)
			f->node->ops->close(f->node);
		kfree(f);
	}
	fd_free(current, fd);
	return (0);
}

ssize_t read(int fd, void *buf, size_t count)
{
	struct file *filed = fd_get(get_current_process(), fd);

	if (filed == NULL || filed->node == NULL || filed->node->ops->read == NULL)
		return (-1);
	char *kbuff = filed->node->ops->read(filed->node);
	if (copy_to_user((char*)buf, kbuff, count) < 0)
	{
		kfree(kbuff);
		return (-1);
	}
	kfree(kbuff);
	return (count);
}

ssize_t	write(int fd, const char *str, size_t count)
{
	(void)fd;
	terminal_write(str, count);
	return (count);
}

