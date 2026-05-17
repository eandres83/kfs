#include "uaccess.h"

ssize_t	copy_to_user(char *user_dst, char *kernel_src, size_t count)
{
	uint32_t addr = (uint32_t)user_dst;
	if (check_addr(addr) == false)
		return (-1);
	kmemcpy(user_dst, kernel_src, count);
	return (0);
}

