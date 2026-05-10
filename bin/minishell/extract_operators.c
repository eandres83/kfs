#include "minishell.h"

static	bool check_input(char **array)
{
	for (int i = 0; array[i]; i++)
	{
		for (int j = 0; array[i][j]; j++)
		{
			if (array[i][j] == '|' || array[i][j] == '<' || array[i][j] == '>')
				return (false);
		}
	}
	return (true);
}

char	**ft_process_input(t_mini *mini, char *line)
{
	char	**array;

	if (!line)
		return (NULL);
	if (ft_count_quotes(line) == -1)
		return (error(mini, 2, "Error: Unclosed quotes"), NULL);
	array = ft_split_prompt(line, ' ');
	if (!array || array == NULL)
	{
		error(mini, 1, "Error: Failed splitting input line");
		return (NULL);
	}
	if (check_input(array) == false)
	{
		ft_free_array(array);
		return (NULL);
	}
	array = ft_expand(mini, array);
	array = ft_final_trim(array);
	return (array);
}

char	**ft_concatenate_array(char **array, char **temp_array, int index)
{
	char	**new_array;
	int		i;
	int		j;

	new_array = malloc(sizeof(char *) * (ft_arraylen(array) + 2));
	if (!new_array)
		return (NULL);
	i = 0;
	while (i < index)
	{
		new_array[i] = ft_strdup(array[i]);
		i++;
	}
	j = 0;
	while (temp_array[j])
		new_array[i++] = ft_strdup(temp_array[j++]);
	while (array[index + 1])
	{
		new_array[i++] = ft_strdup(array[index + 1]);
		index++;
	}
	new_array[i] = NULL;
	ft_free_array(array);
	return (new_array);
}

int	ft_arraylen(char **array)
{
	int	i;

	if (!array || !array[0])
		return (0);
	i = 0;
	while (array && array[i])
	{
		i++;
	}
	return (i);
}

char	**ft_create_temp_array(char **array, int i, int j)
{
	char	**temp_array;

	temp_array = malloc(sizeof(char *) * 3);
	if (!temp_array)
		return (NULL);
	temp_array[0] = ft_substr(&array[i][0], 0, j);
	temp_array[1] = ft_strdup(&array[i][j]);
	temp_array[2] = NULL;
	return (temp_array);
}

void	ft_get_full_envp(t_mini *mini, char **envp)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (envp[count] != NULL)
		count++;
	mini->env_copy = malloc((count + 1) * sizeof(char *));
	if (mini->env_copy == NULL)
		return ;
	while (i < count)
	{
		mini->env_copy[i] = ft_strdup(envp[i]);
		if (mini->env_copy[i] == NULL)
		{
			error(mini, 1, "Could not copy env variables");
			return ;
		}
		i++;
	}
	mini->env_copy[count] = NULL;
}

