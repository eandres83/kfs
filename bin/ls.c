#include "../userland/minilib.h"

int main(int argc, char **argv)
{
	printf("numero de argumentos -> %d\n", argc);
	printf("el primer argumento -> %s\n", argv[0]);

	return (0);
}

