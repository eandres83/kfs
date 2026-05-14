#include "../minilib.h"

char	*substr(const char *s, unsigned int start, size_t len)
{
	char 	*new_str;
	size_t	i = start;
	size_t	j = 0;
	size_t	len_s = strlen(s);

	if (!s)
		return (NULL);
	if (start >= len_s || len == 0 || len_s == 0)
		return (strdup(""));
	while (j < len && s[j + i] != '\0')
		j++;
	new_str = (char*)malloc((sizeof(char) * j) + 1);
	if (!new_str)
		return (NULL);
	for (j = 0; i < len_s && j < len; i++, j++)
		new_str[j] = s[i];
	new_str[j] = '\0';
	return (new_str);
}

