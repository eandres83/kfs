#ifndef VFS_H
#define VFS_H

#include <utils.h>
#include "drivers/io.h"

enum types { VFS_FILE, VFS_DIRECTORY, VFS_BLOCK_DEVICE, VFS_MOUNTPOINT };

struct ops
{
	void (*read)();
	void (*write)();
	void (*open)();
	void (*close)();
	void (*readdir)();
	void (*finddir)();
};

struct vfs_node
{
	char		name[256];
	uint32_t	size;	// tamano en bytes
	uint32_t	inode;
	uint32_t	links; // hard-links
	uint32_t	rights;
	enum types	type;
	struct ops	*ops; // punteros a las func basicas
	struct vfs_node *father;
	struct vfs_node *children;
	struct vfs_node *next_to_kin; // siguiente nodo del hijo
	struct vfs_node *master; // info del nodo
};

#endif
