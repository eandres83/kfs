#include <utils.h>

static char **free_error(char **str)
{
	for (size_t i = 0; str[i]; i++)
		kfree(str[i]);
	kfree(str);
	return (NULL);
}

static size_t nb_words(const char *s, char c)
{
	size_t i = 0;
	size_t nb_words = 0;

	if (!s[0])
		return (0);
	while (s[i] && s[i] == c)
		i++;
	while (s[i])
	{
		if (s[i] == c)
		{
			nb_words++;
			while (s[i] && s[i] == c)
				i++;
			continue;
		}
		i++;
	}
	if (s[i - 1] != c)
		nb_words++;
	return (nb_words);
}

static void get_next_word(char **next_word, size_t *next_word_len, char c)
{
	size_t i = 0;

	*next_word += *next_word_len;
	*next_word_len = 0;

	while (**next_word && **next_word == c)
		(*next_word)++;
	while ((*next_word)[i])
	{
		if ((*next_word)[i] == c)
			return ;
		(*next_word_len)++;
		i++;
	}
}

char **ksplit(const char *s, char c)
{
	char **tab;
	char *next_word;
	size_t next_word_len;
	size_t i;

	if (!s)
		return (NULL);
	tab = (char **)kmalloc(sizeof(char *) * (nb_words(s, c) + 1));
	if (!tab)
		return (NULL);
	i = 0;
	next_word = (char*)s;
	next_word_len = 0;
	while (i < nb_words(s, c))
	{
		get_next_word(&next_word, &next_word_len, c);
		tab[i] = (char*)kmalloc(sizeof(char) * (next_word_len + 1));
		if (!tab[i])
			return (free_error(tab));
		kstrlcpy(tab[i], next_word, next_word_len + 1);
		i++;
	}
	tab[i] = NULL;
	return (tab);
}

