#include "../userland/minilib.h"

int init_module()
{
	return (42);
}

void cleanup_module()
{
	printf("Cleaning module :)\n");
}

