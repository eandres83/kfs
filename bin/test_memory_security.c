#include "../userland/minilib.h"

int main()
{
	void *str = NULL;
	ssize_t res1 = write(0, str, 1);
	if (res1 == -1)
		printf("Bien, no puedes leer NULL :(\n");
	else
		printf("El puto res1 -> %d\n", res1);

	void *str1 = (void*)0xC000A000;
	ssize_t res2 = write(0, str1, 1);
	if (res2 == -1)
		printf("Bien, no puedes leer de una direccion del kernel :(\n");
	else
		printf("El puto res2 -> %d\n", res2);

	void *str2 = (void*)0x44444444;
	ssize_t res3 = write(0, str2, 1);
	if (res3 == -1)
		printf("Bien, no puedes leer una pagina que no esta mapeada :(\n");
	else
		printf("El puto res3 -> %d\n", res3);

	return (0);
}

