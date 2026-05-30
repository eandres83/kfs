#include "arch/i386/lib/uaccess.h"

// copy_to/from_user return the number of bytes that failed
extern uint32_t copy_from_user(void *dest, const void *str, size_t size);
extern uint32_t copy_to_user(void *dest, const void *str, size_t size);

bool	check_addr(uint32_t addr, size_t size)
{
	if (addr >= 0xC0000000)
		return (false);
	size_t res = (size_t)0xC0000000 - addr;
	if (size > res)
		return (false);
	return (true);
}

// to user from kernel
ssize_t	copy_to_user_wrap(void *user_addr, const void *kernel_addr, size_t count)
{
	uint32_t addr = (uint32_t)user_addr;
	if (check_addr(addr, count) == false)
		return (-1);
	uint32_t res = copy_to_user(user_addr, kernel_addr, count);
	if (res > 0)
		return (-1);
	return (res);
}

// to kernel from user
ssize_t copy_from_user_wrap(void *kernel_addr, const void *user_addr, size_t count)
{
	uint32_t addr = (uint32_t)user_addr;
	if (check_addr(addr, count) == false)
		return (-1);
	uint32_t res = copy_from_user(kernel_addr, user_addr, count);
	if (res > 0)
		return (-1);
	return (res);
}

