#include "sysfile.h"
#include "task/task.h"
#include "arch/i386/lib/uaccess.h"
#include "fs/fd.h"

ssize_t dup(int32_t fd)
{
	proc_t *proc = get_current_process();
	if ((fd >= MAX_FD || fd < 0) || proc->fd_table[fd] == NULL)
		return (-1);

	for (int32_t i = 0; i < MAX_FD; i++)
	{
		if (proc->fd_table[i] == NULL)
		{
			proc->fd_table[i] = proc->fd_table[fd];
			proc->fd_table[i]->ref_count++;
			return (i);
		}
	}
	return (-1);
}

ssize_t dup2(int32_t oldfd, int32_t newfd)
{
	proc_t *proc = get_current_process();
	if ((oldfd >= MAX_FD || oldfd < 0) || proc->fd_table[oldfd] == NULL)
		return (-1);
	if (newfd >= MAX_FD || newfd < 0)
		return (-1);

	if (oldfd == newfd)
		return (newfd);
	if (proc->fd_table[newfd] != NULL)
		fd_free(proc, newfd);
	proc->fd_table[newfd] = proc->fd_table[oldfd];
	proc->fd_table[newfd]->ref_count++;
	return (newfd);
}

ssize_t pipe_read(struct vfs_node *node, char *buff, size_t len, size_t offset)
{
	(void)offset;
	struct buf_ring *pipe_buf = (struct buf_ring *)node->fs_info;
	size_t i;
	for (i = 0; len > i; i++)
	{
		if (pipe_buf->count > 0)
		{
			kmemcpy(&buff[i], &pipe_buf->buff[pipe_buf->tail], 1);
			pipe_buf->tail = (pipe_buf->tail + 1) % pipe_buf->size;
			pipe_buf->count--;
		}
	}
	return (i);
}

ssize_t pipe_write(struct vfs_node *node, char *str, size_t size, size_t offset)
{
	(void)offset;
	struct buf_ring *pipe_buf = (struct buf_ring *)node->fs_info;
	size_t i;
	for (i = 0; size > i; i++)
	{
		if (pipe_buf->size > pipe_buf->count)
		{
			kmemcpy(&pipe_buf->buff[pipe_buf->head], &str[i], 1);
			pipe_buf->head = (pipe_buf->head + 1) % pipe_buf->size;
			pipe_buf->count++;
		}
	}
	return (i);
}

size_t	pipe_close(struct vfs_node *node)
{
	struct buf_ring *pipe_buf = (struct buf_ring *)node->fs_info;

	pipe_buf->fds--;
	if (pipe_buf->fds == 0)
	{
		kfree(node);
		kfree(pipe_buf->buff);
	}
	return (0);
}

static struct ops buf_ring = {
	.read = pipe_read,
	.write = pipe_write,
	.open = NULL,
	.close = pipe_close,
	.readdir = NULL,
	.finddir = NULL,
};

ssize_t pipe(int *fd)
{
	int kernel_fd[2];
	proc_t *proc = get_current_process();
	struct vfs_node *node = kmalloc(sizeof(struct vfs_node));
	if (!node)
		return (-1);
	kmemset(node, 0, sizeof(struct vfs_node));
	node->ops = &buf_ring;

	struct buf_ring *buf_ring = kmalloc(sizeof(struct buf_ring));
	if (!buf_ring)
		return (kfree(node), -1);
	kmemset(buf_ring, 0, sizeof(struct buf_ring));
	buf_ring->size = 4096;
	buf_ring->fds = 2;
	node->fs_info = buf_ring;

	kernel_fd[0] = fd_allocate(proc);
	if (kernel_fd[0] == -1)
		return (kfree(buf_ring), kfree(node), -1);
	struct file *file_read = proc->fd_table[kernel_fd[0]];
	file_read->flags = O_RDONLY;
	file_read->node = node;

	kernel_fd[1] = fd_allocate(proc);
	if (kernel_fd[1] == -1)
		return (kfree(buf_ring), kfree(node), fd_free(proc, kernel_fd[0]), -1);
	struct file *file_write = proc->fd_table[kernel_fd[1]];
	file_write->flags = O_WRONLY;
	file_write->node = node;

	if (copy_to_user_wrap(fd, kernel_fd, sizeof(kernel_fd)) == -1)
	{
		fd_free(proc, kernel_fd[0]);
		fd_free(proc, kernel_fd[1]);
		return (kfree(buf_ring), -1);
	}
	kdebug("el valor de los fd asignados -> %d -> %d\n", fd[0], fd[1]);
	return (0);
}

ssize_t	stat_func(const char *path, struct stat *user_stat)
{
	if (!path || !user_stat)
		return (-1);
	struct vfs_node *node = get_vfs_node_path((char*)path);
	if (!node)
		return (-1);
	if (node->ops == NULL || node->ops->stat == NULL)
		return (-1);
	struct stat kernel_stat;
	if (node->ops->stat(node, &kernel_stat) == -1)
		return (-1);
	if (copy_to_user_wrap(user_stat, &kernel_stat, sizeof(struct stat)) == -1)
		return (-1);
	return (0);
}

ssize_t	fstat_func(int fd, struct stat *user_stat)
{
	if (fd < 0 || fd >= MAX_FD || !user_stat)
		return (-1);
	struct file *file = fd_get(get_current_process(), fd);
	if (!file)
		return (-1);
	struct vfs_node *node = file->node;
	if (node->ops == NULL || node->ops->stat == NULL)
		return (-1);
	struct stat kernel_stat;
	if (node->ops->stat(node, &kernel_stat) == -1)
		return (-1);
	if (copy_to_user_wrap(user_stat, &kernel_stat, sizeof(struct stat)) == -1)
		return (-1);
	return (0);
}

ssize_t	lstat_func(const char *path, struct stat *user_stat)
{
	if (!path || !user_stat)
		return (-1);
	struct vfs_node *node = get_vfs_node_path((char*)path);
	if (!node || !node->ops || !node->ops->stat)
		return (-1);
	struct stat kernel_start;
	if (node->ops->stat(node, &kernel_start) == -1)
		return (-1);
	if (copy_to_user_wrap(user_stat, &kernel_start, sizeof(struct stat)) == -1)
		return (-1);
	return (0);
}

ssize_t	lseek(int fd, off_t offset, int whence)
{
	if (fd < 0 || fd >= MAX_FD)
		return (-1);
	struct file *file = fd_get(get_current_process(), fd);
	if (!file)
		return (-1);
	if (whence == SEEK_SET)
		file->offset = offset;
	if (whence == SEEK_CUR)
		file->offset += offset;
	if (whence == SEEK_END)
		file->offset = offset + file->node->size;
	return (file->offset);
}

ssize_t	getdents(int fd, struct linux_dirent *dirp, int count)
{
	if (fd < 0 || fd >= MAX_FD)
		return (-1);
	struct file *file = fd_get(get_current_process(), fd);
	if (!file)
		return (-1);
	struct vfs_node *node = file->node;
	if (node->type != VFS_DIRECTORY)
		return (-1);
	if (node->ops != NULL && node->ops->readdir != NULL)
		node->ops->readdir(node);
	struct vfs_node *current_child = node->children;
	for (uint32_t i = 0; i < file->offset && current_child != NULL; i++)
		current_child = current_child->next_to_kin;

	size_t	bytes_written = 0;
	char *user_ptr = (char*)dirp;
	while (current_child != NULL)
	{
		size_t raw_size = 10 + kstrlen(current_child->name) + 2;
		size_t aligned_size = ((raw_size + 3) / 4) * 4;

		if (bytes_written + aligned_size > (size_t)count)
			break;
		char tmp_buf[256];
		kmemset(tmp_buf, 0, 256);
		struct linux_dirent *tmp_dirent = (struct linux_dirent *)tmp_buf;
		tmp_dirent->d_ino = current_child->inode;
		tmp_dirent->d_off = file->offset + 1;
		tmp_dirent->d_reclen = aligned_size;
		kstrcpy(tmp_dirent->d_name, current_child->name);

		char d_type = 8;
		if (current_child->type == VFS_DIRECTORY)
			d_type = 4;
		else if (current_child->type == VFS_SYMLINK)
			d_type = 10;

		tmp_buf[aligned_size - 1] = d_type;
		if (copy_to_user_wrap(user_ptr, tmp_buf, aligned_size) == -1)
			return (-1);
		user_ptr += aligned_size;
		bytes_written += aligned_size;
		file->offset++;
		current_child = current_child->next_to_kin;
	}
	return (bytes_written);
}

