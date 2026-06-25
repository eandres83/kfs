#include <utils.h>

size_t	katoi(char *str)
{
	char *tmp = str;
	int result = 0;
	int sign = 1;

	while (*tmp == 32 || (*tmp >= 9 && *tmp <= 13))
		tmp++;
	if (*tmp == '-' || *tmp == '+')
	{
		if (*tmp == '-')
			sign *= -1;
		tmp++;
	}
	while (*tmp >= '0' && *tmp <= '9')
	{
		result = result * 10 + *tmp - '0';
		tmp++;
	}
	return (result * sign);
}

size_t _api_katoi(char *str) __attribute__((alias("katoi")));

