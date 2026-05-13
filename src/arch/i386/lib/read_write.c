#include "arch/i386/lib/uaccess.h"

ssize_t	open(char *pathname, uint32_t flags)
{
	proc_t *current = get_current_process();

	int fd = fd_allocate(current);
	if (fd < 0)
		return (-24);

	struct vfs_node *node = get_vfs_node_path(pathname);
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

	char *kbuff = filed->node->ops->read(filed->node);
	for (size_t i = 0; i < count; i++)
		kbuff[i] = tty_read_char();

	copy_to_user(kbuff, (char*)buf, count);
	return (count);
}

ssize_t	write(int fd, const char *str, size_t count)
{
	if (fd != 1)
		return (-1);
	terminal_write(str, count);
	return (count);
}

