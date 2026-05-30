#include "../userland/minilib.h"

int main()
{
	int fd[2];
	char buffer[100];
	int status;

	if (pipe(fd) == -1)
	{
		printf("Fallo critico: No se pudo crear el pipe en el kernel.\n");
		return (1);
	}
	printf("Pipe creado: fd_read=%d, fd_write=%d\n", fd[0], fd[1]);

	ssize_t pid = fork();

	if (pid < 0)
	{
		printf("Fallo critico: Error en el fork.\n");
		return (1);
	}

	if (pid == 0)
	{
		close(fd[0]); // El hijo no va a leer cerramos su extremo de lectura

		char *msg = "Hola padre, el pipe funciona perfectamente!";
		ssize_t bytes_escritos = write(fd[1], msg, 43); 

		if (bytes_escritos > 0)
			printf("Hijo: He escrito %d bytes en el pipe.\n", (int)bytes_escritos);
		else
			printf("Hijo: Fallo al escribir en el pipe.\n");

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
			printf("Padre: Recibi el mensaje -> '%s'\n", buffer);
		}
		else
			printf("Padre: No se leyo nada o hubo un error en read.\n");

		close(fd[0]);
		wait(&status);
		printf("Test finalizado con exito.\n");
	}

	return (0);
}

