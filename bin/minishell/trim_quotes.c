#include "minishell.h"

int	count_quotes(char const *s1)
{
	int		count;
	int		i;
	bool	dquote;
	bool	squote;

	if (!s1)
		return (0);
	i = 0;
	count = 0;
	dquote = 0;
	squote = 0;
	while (s1[i] != '\0')
	{
		squote = (squote + (!dquote && s1[i] == '\'')) % 2;
		dquote = (dquote + (!squote && s1[i] == '\"')) % 2;
		if ((s1[i] == '\"' && !squote) || (s1[i] == '\'' && !dquote))
			count++;
		i++;
	}
	if (squote || dquote)
		return (-1);
	return (count);
}

char	*strtrim_quotes(char const *s1, int squote, int dquote)
{
	int		count;
	int		i[2];
	char	*trimmed;

	i[1] = -1;
	i[0] = 0;
	count = count_quotes(s1);
	if (!s1 || count == -1)
		return (NULL);
	trimmed = malloc(sizeof(char) * (strlen(s1) - count + 1));
	if (!trimmed)
		return (NULL);
	while (s1[i[0]])
	{
		squote = (squote + (!dquote && s1[i[0]] == '\'')) % 2;
		dquote = (dquote + (!squote && s1[i[0]] == '\"')) % 2;
		if ((s1[i[0]] != '\"' || squote) && (s1[i[0]] != '\'' || dquote) \
			&& ++i[1] >= 0)
			trimmed[i[1]] = s1[i[0]];
		i[0]++;
	}
	trimmed[++i[1]] = '\0';
	return (trimmed);
}

char	quote_context(const char *s, int index)
{
	char		quote;
	const char	*aux;
	int			i;

	quote = 0;
	aux = s;
	i = 0;
	while (i < index)
	{
		if (aux[i] == '\'' || aux[i] == '\"')
		{
			if (quote == 0)
				quote = aux[i];
			else if (quote == aux[i])
				quote = 0;
		}
		i++;
	}
	return (quote);
}

char	**final_trim(char **array)
{
	int		i;
	char	*trimmed;
	int		len;

	if (!array)
		return (NULL);
	i = 0;
	while (array[i])
	{
		len = strlen(array[i]);
		if ((len > 1) && array[i][0] == array[i][len - 1])
		{
			if (array[i][0] == '\'' || array[i][0] == '\"')
			{
				trimmed = substr(array[i], 1, (len - 2));
				free(array[i]);
				array[i] = trimmed;
			}
		}
		i++;
	}
	return (array);
}

