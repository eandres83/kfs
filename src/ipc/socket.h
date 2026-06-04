#ifndef SOCKET_H
#define SOCKET_H

#include <utils.h>
#include "fs/vfs/vfs.h"
#include "fs/sysfile.h"
#include "task/task.h"

#define O_SOCKET_SIDE_A 1
#define O_SOCKET_SIDE_B 2

struct socket_direct
{
	struct buf_ring text_buff;
	struct file	*shared_fds[8];
	proc_t	*waiting_process;
};

struct socket_channel
{
	struct socket_direct socket_ab;
	struct socket_direct socket_ba;
	uint32_t	fds_count;
};

ssize_t socketpair(int domain, int type, int protocol, int *sv);
ssize_t sendmsg(int fd_socket, const struct msghdr *msg, int flags);
ssize_t	recvmsg(int fd_socket, struct msghdr *msg, int flags);

#endif
