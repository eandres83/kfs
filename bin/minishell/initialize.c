#include "minishell.h"

int	locate_pipe(char **array, int *index)
{
	int	i;

	i = 1;
	while (array && array[i])
	{
		if (strcmp(array[i], "|") == 0 && array[i+1] != NULL)
		{
			*index = *index + i + 1;
			return (1);
		}
		else
			i++;
	}
	return (0);
}

void	get_full_command(t_mini *node, char **array)
{
	int	i;
	int	j;
	int	k;

	k = 0;
	if (node->limit != NULL)
		k += 3;
	else if (array && array[k] && *array[k] == '<')
		k += 2;
	i = 0;
	while (array && array[k] && (*array[k] != '|' && *array[k] != '>'))
	{
		k++;
		i++;
	}
	node->full_cmd = malloc(sizeof(char *) * (i + 1));
	if (!node->full_cmd)
		return ;
	j = -1;
	while (++j < i)
	{
		node->full_cmd[j] = strdup(array[k - i]);
		k++;
	}
	node->full_cmd[j] = NULL;
}

bool	check_redirections(t_mini *node, char **array)
{
	int	i = 0;

	if (!array || !array[i])
		return (true);
	if (*array[i] == '<' && array[i + 1] && *array[i + 1] == '<'
		&& array[i + 2])
	{
		node->limit = strdup(array[i + 2]);
//		create_tmp(node);
	}
	else if (array && array[i] && *array[i] == '<')
		node->infile = open(array[i + 1], 1, 1);
//		node->infile = open(array[i + 1], O_RDONLY);
	while (array[i])
	{
		if (*array[i] == '|')
			return (false);
		else
			i++;
	}
	return (check_redirections_util(node, array));
}

t_mini	*initialize_mini_node(char **envp)
{
	t_mini	*new_node;

	new_node = malloc(sizeof(t_mini));
	if (!new_node)
		return (NULL);
	memset(new_node, 0, sizeof(t_mini));
	get_full_envp(new_node, envp);
	new_node->infile = STDIN_FILENO;
	new_node->outfile = STDOUT_FILENO;
	new_node->next = NULL;
	new_node->command = NULL;
	new_node->full_cmd = NULL;
	new_node->full_path = NULL;
	return (new_node);
}

t_mini	*create_structure(t_mini *mini, char **array, char **envp)
{
	t_mini	*head;
	t_mini	*next_node;
	int	index = 0;

	head = mini;
	while (1)
	{
		if (!check_redirections(mini, &array[index]))
			break;
		get_full_command(mini, &array[index]);
		check_if_builtin(mini);
		get_path(mini);
		if (!locate_pipe(&array[index], &index))
			break ;
		next_node = initialize_mini_node(envp);
		mini->next = next_node;
		mini = next_node;
	}
	return (head);
}

