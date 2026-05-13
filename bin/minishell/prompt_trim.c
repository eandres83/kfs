#include "../minishell.h"

static size_t	ft_handle_quotes(const char *s, size_t i, char quote)
{
	i++;
	while (s[i] && s[i] != quote)
		i++;
	if (s[i] == quote)
		return (i + 1);
	else
		return (i);
}

static size_t	ft_nb_words(char const *s, char c)
{
	size_t	i;
	size_t	nb_words;

	if (!s[0])
		return (0);
	i = 0;
	nb_words = 0;
	while (s[i] && s[i] == c)
		i++;
	while (s[i])
	{
		if (s[i] == '\'' || s[i] == '\"')
			i = ft_handle_quotes(s, i, s[i]);
		else if (s[i] == c)
		{
			nb_words++;
			while (s[i] && s[i] == c)
				i++;
		}
		else
			i++;
	}
	if (s[i - 1] != c)
		nb_words++;
	return (nb_words);
}

static void	ft_get_next_word(char **next_word, size_t *next_word_len, char c)
{
	size_t	i;
	char	quote;

	*next_word += *next_word_len;
	*next_word_len = 0;
	i = 0;
	while (**next_word && **next_word == c)
		(*next_word)++;
	while ((*next_word)[i])
	{
		if ((*next_word)[i] == '\'' || (*next_word)[i] == '\"')
		{
			quote = (*next_word)[i];
			i++;
			while ((*next_word)[i] && (*next_word)[i] != quote)
				i++;
			if ((*next_word)[i] == quote)
				i++;
		}
		else if ((*next_word)[i] == c)
			break ;
		else
			i++;
	}
	*next_word_len = i;
}

char	**ft_split_prompt(char const *s, char c)
{
	char	**array;
	char	*next_word;
	size_t	next_word_len;
	size_t	i;

	if (!s)
		return (NULL);
	array = (char **)malloc(sizeof(char *) * (ft_nb_words(s, c) + 1));
	if (!array)
		return (NULL);
	i = 0;
	next_word = (char *)s;
	next_word_len = 0;
	while (i < ft_nb_words(s, c))
	{
		ft_get_next_word(&next_word, &next_word_len, c);
		array[i] = (char *)malloc(sizeof(char) * (next_word_len + 1));
		if (!array[i])
			return (NULL);
		ft_strlcpy(array[i], next_word, next_word_len + 1);
		i++;
	}
	array[i] = NULL;
	return (array);
}

