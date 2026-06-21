#include "../userland/minilib.h"

int main()
{
//	char *str = (char*)0xFFFFF000;
	char *str1 = "/bin/dummy_module.ko";
	ssize_t res = init_module(str1);
	if (res == -1)
		return (printf("Miredon el modulo broo; res -> %d\n", res), -1);
	ssize_t	res1 = del_module("dummy_module.ko");
	if (res1 == -1)
		return (printf("Otro mierdon el del module acho; res -> %d\n", res1), -1);
	ssize_t res2 = init_module("/bin/dummy_module.ko");
	if (res2 == -1)
		return (printf("Miredon el modulo broo; res -> %d\n", res2), -1);
	return (0);
}

