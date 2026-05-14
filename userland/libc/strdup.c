#include "../minilib.h"

char	*strdup(const char *str)
{
	int len;
	char *dest;
	char *temp_src;

	len = strlen(str) + 1;
	temp_src = (char*)str;
	dest = (char*)malloc(sizeof(char) * len);
	if (!dest)
		return (0);
	strcpy(dest, temp_src);
	return (dest);
}

