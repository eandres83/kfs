#ifndef UAPI_H
#define UAPI_H

#include <sys/types.h>

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// open flags
#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR	 2

// access mode
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4

// waitpid options
#define WNOHANG 1

// socket struct and defines
struct iovec
{
	void	*iov_base; // Starting address
	size_t	iov_len;   // size of the memory pointed to by iov_base
};

struct msghdr
{
	void		*msg_name;	// address
	socklen_t	len_namelen;	// size of address
	struct iovec	*msg_iov;	// scatter/gather vector
	size_t		msg_iovlen;	// elements in msg_iov
	void		*msg_control;	// ancillary data
	size_t		msg_controllen;	// ancillary data buffer size
	int32_t		msg_flags;	// flags on received message
};

// socket flags
#define AF_UNIX  1
#define AF_LOCAL 1

#define SOCK_STREAM 2

#define SCM_RIGHTS 0x01

// lseek flags
#define SEEK_SET	1
#define SEEK_CUR	2
#define SEEK_END	3

struct timespec
{
	time_t	tv_sec;
	int32_t	tv_nsec;
};

struct stat
{
	dev_t		st_dev;
	ino_t		st_ino;
	mode_t		st_mode;
	nlink_t		st_nlink;
	uid_t		st_uid;
	gid_t		st_gid;
	dev_t		st_rdev;
	off_t		st_size;
	blksize_t	st_blksize;
	blkcnt_t	st_blocks;
	struct timespec	st_atim;
	struct timespec	st_mtim;
	struct timespec	st_ctim;
};

// getdents
struct linux_dirent
{
	uint32_t	d_ino;		// inode number
	uint32_t	d_off;		// offset
	uint16_t	d_reclen;	// length fo this linux_dirent
	char		d_name[];	// filename (null-terminated)
};

// mprotect flags
#define PROT_NONE	0
#define PROT_READ	1
#define PROT_WRITE	2
#define PROT_EXEC	4

#endif
