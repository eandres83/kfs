#include "../userland/minilib.h"

int main()
{
	ssize_t fd = open("/etc/shadow", O_RDONLY, 1);
	if (fd < 0)
		return (printf("Error: cannot open file :(\n"), 1);
	printf("GOOD, you can open /etc/shadow congrat\n");
	return (0);
}

