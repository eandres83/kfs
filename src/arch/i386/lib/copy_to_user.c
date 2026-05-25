#include "uaccess.h"

// to user from kernel
ssize_t	copy_to_user(void *user_addr, const void *kernel_addr, size_t count)
{
	uint32_t addr = (uint32_t)user_addr;
	if (check_addr(addr) == false)
		return (-1);
	kmemcpy(user_addr, kernel_addr, count);
	return (0);
}

// to kernel from user
size_t copy_from_user(void *kernel_addr, const void *user_addr, size_t count)
{
	uint32_t addr = (uint32_t)user_addr;
	if (check_addr(addr) == false)
		return (-1);
	kmemcpy(kernel_addr, user_addr, count);
	return (0);
}

