#include "minishell.h"

int	ft_count_quotes(char const *s1)
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

char	*ft_strtrim_quotes(char const *s1, int squote, int dquote)
{
	int		count;
	int		i[2];
	char	*trimmed;

	i[1] = -1;
	i[0] = 0;
	count = ft_count_quotes(s1);
	if (!s1 || count == -1)
		return (NULL);
	trimmed = malloc(sizeof(char) * (ft_strlen(s1) - count + 1));
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

char	ft_quote_context(const char *s, int index)
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

char	**ft_final_trim(char **array)
{
	int		i;
	char	*trimmed;
	int		len;

	if (!array)
		return (NULL);
	i = 0;
	while (array[i])
	{
		len = ft_strlen(array[i]);
		if ((len > 1) && array[i][0] == array[i][len - 1])
		{
			if (array[i][0] == '\'' || array[i][0] == '\"')
			{
				trimmed = ft_substr(array[i], 1, (len - 2));
				free(array[i]);
				array[i] = trimmed;
			}
		}
		i++;
	}
	return (array);
}

void	ft_handle_sigint(int signal)
{
	(void)signal;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	update_exit_status(130);
}

void	update_exit_status(int status)
{
	if (WIFEXITED(status))
		g_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		g_exit_status = 128 + WTERMSIG(status);
}

int	get_exit_status(void)
{
	return (g_exit_status);
}

void	setup_signals(void)
{
	signal(SIGINT, ft_handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}

