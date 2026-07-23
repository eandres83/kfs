#include <utils.h>
#include <sys/kmalloc.h>

void	double_free(char **str)
{
	for (int i = 0; str[i]; i++)
		kfree(str[i]);
	kfree(str);
}

