#include "minishell.h"

char	*expand_path(char *word)
{
	const char	*home_dir;
	char		*expanded;

	if (word[0] != '~')
		return (strdup(word));
//	home_dir = get_env_value(
	home_dir = strdup("mi/home");
	if (!home_dir)
		home_dir = "";
	expanded = malloc(strlen(home_dir) + strlen(word));
	if (!expanded)
		return (NULL);
	strlcpy(expanded, home_dir, (strlen(home_dir) + 1));
	strlcat(expanded, word + 1, (strlen(expanded) + strlen(word)));
	return (expanded);
}

static char	*get_env_value(t_mini *mini, const char *name)
{
	int	i;
	int	len;

	len = strlen(name);
	i = 0;
	while (mini->env_copy[i])
	{
		if (strncmp(mini->env_copy[i], name, len) == 0
			&& mini->env_copy[i][len] == '=')
		{
			return (mini->env_copy[i] + len + 1);
		}
		i++;
	}
	return (NULL);
}

char	*expand_variable(t_mini *mini, char *word, int index)
{
	char	*var_name;
	int	name_len;
	char	*var_value;
	char	*expanded;
	char	*temp_exp;

	if (strncmp("$?", word, 2) == 0)
		return (itoa(g_exit_status));
	name_len = get_var_name_len(&word[index + 1]);
	var_name = substr(&word[index + 1], 0, name_len);
	var_value = get_env_value(mini, var_name);
	free(var_name);
	if (!var_value)
		var_value = "";
	expanded = malloc(strlen(var_value) + 1);
	if (!expanded)
		return (NULL);
	strlcpy(expanded, var_value, (strlen(var_value) + 1));
	if (word[index + name_len + 1] != '\0')
	{
		var_value = &word[index + name_len + 1];
		temp_exp = expanded;
		expanded = strjoin(temp_exp, var_value);
		free(temp_exp);
	}
	return (expanded);
}

int	get_var_name_len(const char *var_name)
{
	int	len;

	len = 0;
	while (*var_name && (isalnum(*var_name) || *var_name == '_'))
	{
		len++;
		var_name++;
	}
	return (len);
}

char	**expand(t_mini *mini, char **temp)
{
	int	i;
	int	j;
	char	*prefix;
	char	*aux;
	char 	*new_str;

	i = -1;
	while (temp && temp[++i])
	{
		j = -1;
		while (temp[i][++j])
		{
			if (temp[i][0] == '~')
				temp[i] = expand_path(temp[i]);
			else if (temp[i][j] == '$' && quote_context(temp[i], j) != '\'')
			{
				prefix = substr(temp[i], 0, j);
				aux = expand_variable(mini, temp[i], j);
				if (aux == NULL)
					aux = strdup("");
				new_str = strjoin(prefix, aux);
				free(temp[i]);
				temp[i] = new_str;
				free(aux);
				free(prefix);
			}
		}
	}
	return (temp);
}

