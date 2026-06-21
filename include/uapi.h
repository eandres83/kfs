#ifndef UAPI_H
#define UAPI_H

typedef int32_t ssize_t;
typedef uint32_t socklen_t;

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

// syscall
#define SYS_exit 	1
#define SYS_fork 	2
#define SYS_read 	3
#define SYS_write	4
#define SYS_open	5
#define SYS_close	6
#define SYS_waitpid	7
#define SYS_wait	8
#define SYS_execve	11
#define SYS_chdir	12
#define SYS_setuid	23
#define SYS_getuid	24
#define SYS_access	33
#define SYS_kill	37
#define SYS_dup		41
#define SYS_pipe	42
#define SYS_setgid	46
#define SYS_getgid	47
#define SYS_signal	48
#define SYS_dup2	63
#define SYS_mmap	90
#define SYS_munmap	91
#define SYS_init_module 128
#define SYS_del_module	129
#define SYS_getcwd	183
#define SYS_socketpair	360
#define SYS_sendmsg	370
#define SYS_recvmsg	372

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

#define AF_UNIX  1
#define AF_LOCAL 1

#define SOCK_STREAM 2

#define SCM_RIGHTS 0x01

#endif
