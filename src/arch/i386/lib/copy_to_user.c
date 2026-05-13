#include "uaccess.h"

ssize_t	copy_to_user(char *kernel, char *str, size_t count)
{
	uint32_t addr = (uint32_t)str;
	if (check_addr(addr) == false)
		return (-1);
	kstrlcpy(str, kernel, count);
	return (0);
}

