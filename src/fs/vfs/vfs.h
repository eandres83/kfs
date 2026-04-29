#ifndef VFS_H
#define VFS_H

#include <utils.h>
#include <kmalloc.h>
#include "drivers/io.h"
#include "task/task.h"

extern struct vfs_node *vfs;

enum types { VFS_UNKNOWN, VFS_FILE, VFS_DIRECTORY, VFS_BLOCK_DEVICE, VFS_MOUNTPOINT};

struct vfs_node
{
	char		name[126];
	uint32_t	size;	// tamano en bytes
	uint32_t	inode;
	uint32_t	links; // hard-links
	uint32_t	rights;
	void		*fs_info;
	enum types	type;
	struct ops	*ops; // punteros a las func basicas
	struct vfs_node *father;
	struct vfs_node *children;
	struct vfs_node *next_to_kin; // siguiente nodo del hijo
	struct vfs_node *master; // info del nodo
};

struct ops
{
	char *(*read) (struct vfs_node *);
	size_t (*write) (struct vfs_node *, char *, char *);
	void (*open) (struct vfs_node *);
	void (*close) (struct vfs_node *);
	void (*readdir) (struct vfs_node *);
	struct vfs_node *(*finddir) (struct vfs_node *, char *);
};

void init_vfs();
struct vfs_node *get_vfs_node_path(char *path);

#endif
