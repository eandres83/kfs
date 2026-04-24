#include <utils.h>

size_t	kstrncmp(const char *s1, const char *s2, size_t n)
{
	size_t i = 0;

	if (n == 0)
		return (0);
	while (i < n)
	{
		if (s1[i] && s1[i] == s2[i])
		{
			while (s1[i] && s1[i] == s2[i] && i < n)
				i++;
		}
		else
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
	}
	return (0);
}

