#include "../minilib.h"

int 	isdigit(char c)
{
	if (c >= '0' && c <= '9')
		return (1);
	return (0);
}

int	isalpha(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	return (0);
}

int	isalnum(char c)
{
	if (isalpha(c) && isdigit(c))
		return (1);
	return (0);
}

