#ifndef SYS_H
#define SYS_H

uid_t setuid(uint32_t uid);
uid_t getuid();
gid_t setgid(uint32_t gid);
gid_t	getgid();

uid_t	geteuid();
gid_t	getegid();

uid_t	getuid32();
gid_t	getgid32();
uid_t	setuid32(uint32_t uid);
gid_t	setgid32(uint32_t gid);

uid_t	geteuid32();
gid_t	getegid32();

char	*getcwd(char *buf, size_t size);
ssize_t	chdir(char *path);

int ioctl(int fd, int cmd, int arg);

#endif
