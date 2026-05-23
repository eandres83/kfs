#include "../minilib.h"

void	dfree(char **str)
{
	if (!str || !str[0])
		return ;
	for (int i = 0; str[i]; i++)
		free(str[i]);
	free(str);
}

