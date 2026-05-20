#ifndef UAPI_H
#define UAPI_H

typedef int32_t ssize_t;

#define STDIN_FILENO 0
#define STDOUT_FILENO 1

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

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
#define SYS_getuid	24
#define SYS_access	33
#define SYS_kill	37
#define SYS_dup		41
#define SYS_pipe	42
#define SYS_signal	48
#define SYS_dup2	63
#define SYS_mmap	90
#define SYS_munmap	91
#define SYS_getcwd	183

#endif
