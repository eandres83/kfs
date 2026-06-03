#ifndef SOCKET_H
#define SOCKET_H

#include <utils.h>
#include "fs/vfs/vfs.h"
#include "fs/sysfile.h"

#define O_SOCKET_SIDE_A 1
#define O_SOCKET_SIDE_B 2

//struct iovec
//{
//	void	*iov_base; // Starting address
//	size_t	iov_len;   // size of the memory pointed to by iov_base
//};
//
//struct msghdr
//{
//	void		*msg_name;	// address
//	socklen_t	len_namelen;	// size of address
//	struct iovec	*msg_iov;	// scatter/gather vector
//	size_t		msg_iovlen;	// elements in msg_iov
//	void		*msg_control;	// ancillary data
//	size_t		msg_controllen;	// ancillary data buffer size
//	int32_t		msg_flags;	// flags on received message
//};

struct socket_direct
{
	struct buf_ring text_buff;
	struct file	*shared_fds[8];
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
