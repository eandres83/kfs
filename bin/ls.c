#include "../userland/minilib.h"

int main(int argc, char **argv, char **env)
{
	(void)argc;
	(void)env;
	printf("el argv[0] -> %s\n", argv[0]);
	write(1, "HOLA\n", 5);
	return (0);
}

