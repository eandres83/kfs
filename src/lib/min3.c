#include <utils.h>

uint32_t min3(uint32_t num1, uint32_t num2, uint32_t num3)
{
	if ((num1 < num2) && (num1 < num3))
		return (num1);
	else if (num2 < num3)
		return (num2);
	return (num3);
}

