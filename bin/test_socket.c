#include "../userland/minilib.h"

int main()
{
	int status;
	int socket[2];
	ssize_t res = socketpair(AF_UNIX, SOCK_STREAM, 0, socket);
	if (res == -1)
		return (printf("Puto erro en socketpair :(\n"), 1);
	ssize_t	pid = fork();
	if (pid < 0)
		return (printf("Error puto fork\n"), 1);

	if (pid == 0)
	{
		close(socket[1]);
		struct msghdr *msg = malloc(sizeof(struct msghdr));
		if (!msg)
			return (-1);
		memset(msg, 0, sizeof(struct msghdr));
		struct iovec *vec = malloc(sizeof(struct iovec));
		if (!vec)
			return (-1);
		memset(vec, 0, sizeof(struct iovec));
		char buff[100];
		vec->iov_base = buff;
		vec->iov_len = 100;
		msg->msg_iov = vec;
		msg->msg_iovlen = 1;
		ssize_t recv = recvmsg(socket[0], msg, 0);
		if (recv == -1)
			return (printf("erro del puto recvmsg \n"), 1);
		write(1, (char*)msg->msg_iov->iov_base, recv);
		memset(msg, 0, sizeof(struct msghdr));
		memset(vec, 0, sizeof(struct iovec));
		vec->iov_base = "PONG";
		vec->iov_len = strlen(vec->iov_base);
		msg->msg_iov = vec;
		msg->msg_iovlen = 1;
		if (sendmsg(socket[0], msg, 0) == -1)
			return (printf("error en el sendmsg del hijo\n"), 1);
	}
	else if (pid > 0)
	{
		close(socket[0]);
		struct msghdr *msg = malloc(sizeof(struct msghdr));
		if (!msg)
			return (-1);
		memset(msg, 0, sizeof(struct msghdr));
		struct iovec *vec = malloc(sizeof(struct iovec));
		if (!vec)
			return (-1);
		vec->iov_base = "PING\n";
		vec->iov_len = strlen(vec->iov_base);
		msg->msg_iov = vec;
		msg->msg_iovlen = 1;
		if (sendmsg(socket[1], msg, 0) == -1)
			return (printf("error de la putita sendmsg\n"), 1);

		memset(msg, 0, sizeof(struct msghdr));
		memset(vec, 0, sizeof(struct iovec));
		char buff[100];
		vec->iov_base = buff;
		vec->iov_len = 100;
		msg->msg_iov = vec;
		msg->msg_iovlen = 1;
		ssize_t recv = recvmsg(socket[1], msg, 0);
		if (recv == -1)
			return (printf("Error en el recvmsg del padre\n"), 1);
		write(1, (char*)msg->msg_iov->iov_base, recv);
		wait(&status);
	}
	return (0);
}

// test send fd
//int main()
//{
//	int status;
//	int socket[2];
//	ssize_t res = socketpair(AF_UNIX, SOCK_STREAM, 0, socket);
//	if (res == -1)
//		return (printf("Puto erro en socketpair :(\n"), 1);
//	ssize_t	pid = fork();
//	if (pid < 0)
//		return (printf("Error puto fork\n"), 1);
//	if (pid == 0)
//	{
//		close(socket[1]);
//		int fd;
//		struct msghdr *msg = malloc(sizeof(struct msghdr));
//		if (!msg)
//			return (-1);
//		memset(msg, 0, sizeof(struct msghdr));
//		struct iovec *vec = malloc(sizeof(struct iovec));
//		if (!vec)
//			return (-1);
//		memset(vec, 0, sizeof(struct iovec));
//		msg->msg_control = &fd;
//		msg->msg_controllen = sizeof(fd);
//		ssize_t recv = recvmsg(socket[0], msg, SCM_RIGHTS);
//		if (recv == -1)
//			return (printf("Error en el recv del hijo\n"), 1);
//		char buff[100];
//		ssize_t res = read(fd, buff, 100);
//		if (res == -1)
//			return (printf("Error en el read\n"), 1);
//		printf("intento de leer -> %s", buff);
//	}
//	if (pid > 0)
//	{
//		close(socket[0]);
//		struct msghdr *msg = malloc(sizeof(struct msghdr));
//		if (!msg)
//			return (-1);
//		memset(msg, 0, sizeof(struct msghdr));
//		struct iovec *vec = malloc(sizeof(struct iovec));
//		if (!vec)
//			return (-1);
//		memset(vec, 0, sizeof(struct iovec));
//		ssize_t	fd = open("/home/kfs/file.txt", O_RDONLY, 0);
//		if (fd < 0)
//			return (printf("otro puto erro en el open\n"), 1);
//		msg->msg_control = (void*)fd;
//		msg->msg_controllen = 1;
//		if (sendmsg(socket[1], msg, SCM_RIGHTS) == -1)
//			return (printf("otro puto erro el sendmsg\n"), 1);
//		wait(&status);
//	}
//	close(socket[0]);
//	close(socket[1]);
//	return (0);
//}
//
