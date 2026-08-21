#ifndef SYSCALL_H
#define SYSCALL_H

#define SYS_exit 		1
#define SYS_fork 		2
#define SYS_read 		3
#define SYS_write		4
#define SYS_open		5
#define SYS_close		6
#define SYS_waitpid		7
#define SYS_wait		8
#define SYS_execve		11
#define SYS_chdir		12
#define SYS_lseek		19
#define SYS_getpid		20
#define SYS_setuid		23
#define SYS_getuid		24
#define SYS_access		33
#define SYS_kill		37
#define SYS_dup			41
#define SYS_pipe		42
#define SYS_brk			45
#define SYS_setgid		46
#define SYS_getgid		47
#define SYS_signal		48
#define SYS_geteuid		49
#define SYS_getegid		50
#define SYS_ioctl		54
#define SYS_dup2		63
#define SYS_symlink		83
#define SYS_readlink		85
#define SYS_mmap		90
#define SYS_munmap		91
#define SYS_stat		106
#define SYS_lstat		107
#define SYS_fstat		108
#define SYS_mprotect		125
#define SYS_init_module 	128
#define SYS_del_module		129
#define SYS_getdents		141
#define SYS_getcwd		183
#define SYS_getuid32		199
#define SYS_getgid32		200
#define SYS_geteuid32		201
#define SYS_getegid32		202
#define SYS_setuid32		213
#define SYS_setgid32		214
#define SYS_socketpair		360
#define SYS_sendmsg		370
#define SYS_recvmsg		372

#endif
