#include "../userland/minilib.h"

int main()
{
	int fd[2];
	char buffer[100];
	int status;

	if (pipe(fd) == -1)
	{
		printf("Critical failure: pipe could no be created\n");
		return (1);
	}
	printf("Pipe created: fd_read=%d, fd_write=%d\n", fd[0], fd[1]);

	ssize_t pid = fork();

	if (pid < 0)
	{
		printf("Critical failure: Error in the fork.\n");
		return (1);
	}

	if (pid == 0)
	{
		close(fd[0]); // El hijo no va a leer cerramos su extremo de lectura

		char *msg = "Hi father, the pipe is working fine!\n";
		ssize_t bytes_escritos = write(fd[1], msg, strlen(msg)); 

		if (bytes_escritos > 0)
			printf("Son: I wrote %d bytes in the pipe.\n", (int)bytes_escritos);
		else
			printf("Son: Failed to write in the pipe.\n");

		close(fd[1]);
		exit(0);
	}
	else
	{
		close(fd[1]); // El padre no va a escribir cerramos su extremo de escritura

		ssize_t bytes_leidos = read(fd[0], buffer, 99);

		if (bytes_leidos > 0)
		{
			buffer[bytes_leidos] = '\0';
			printf("Father: reciv the message -> '%s'\n", buffer);
		}
		else
			printf("Father: I couldn't read something.\n");

		close(fd[0]);
		wait(&status);
		printf("Finish tests.\n");
	}

	return (0);
}

