#include "arch/i386/lib/uaccess.h"
#include "fs/ext2/ext2.h"
#include "task/task.h"

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
	if (node->ops && node->ops->open)
	{
		int err = node->ops->open(node, flags);
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

ssize_t access(char *filename, int mode)
{
	struct vfs_node *node = get_vfs_node_path(filename);
	if (node == 0x0)
		return (-1);
	struct ext2_inode *inode = get_inode(node);
	if (!inode)
		return (-1);
	proc_t *current_process = get_current_process();

	if (current_process->uid == 0)
		return (0);
	if (current_process->uid == inode->user_id)
	{
		if (check_permission(inode->type_permisi, 6, mode) == -1)
			return (-1);
	}
	else if (current_process->gid == inode->group_id)
	{
		if (check_permission(inode->type_permisi, 3, mode) == -1)
			return (-1);
	}
	else
	{
		if (check_permission(inode->type_permisi, 0, mode) == -1)
			return (-1);
	}
	return (0);
}

