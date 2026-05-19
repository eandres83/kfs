#include "minishell.h"

void	check_if_builtin(t_mini *node)
{
	if (!strcmp(node->full_cmd[0], "echo"))
		node->is_builtin = 1;
	else if (!strcmp(node->full_cmd[0], "cd"))
		node->is_builtin = 1;
	else if (!strcmp(node->full_cmd[0], "pwd"))
		node->is_builtin = 1;
	else if (!strcmp(node->full_cmd[0], "export"))
		node->is_builtin = 1;
	else if (!strcmp(node->full_cmd[0], "unset"))
		node->is_builtin = 1;
	else if (!strcmp(node->full_cmd[0], "env"))
		node->is_builtin = 1;
	else if (!strcmp(node->full_cmd[0], "exit"))
		node->is_builtin = 1;
}

static void	check_this(t_mini *node)
{
	if (node->full_cmd[0] && (node->full_cmd[0][0] == '/'
		|| node->full_cmd[0][0] == '.'))
	{
		node->full_path = strdup(node->full_cmd[0]);
		return ;
	}
}

static	char	**get_path_util(t_mini *node, char **paths)
{
	int	i;

	check_this(node);
	i = 0;
	while (node->env_copy[i])
	{
		if (strncmp("PATH=", node->env_copy[i], 5) == 0)
		{
			paths = split(&(node->env_copy[i][5]), ':');
			break ;
		}
		else
			i++;
	}
	if (!paths)
		return (NULL);
	return (paths);
}

void	get_path(t_mini *node)
{
	char	**paths;
	char	*temp_path;
	char	*valid_path;
	int	i;

	if (node->is_builtin == 1)
		return ;
	paths = NULL;
	paths = get_path_util(node, paths);
	if (node->full_path != NULL)
	{
		free_array(paths);
		return ;
	}
	i = 0;
	while (paths && paths[i])
	{
		temp_path = strjoin(paths[i++], "/");
		valid_path = strjoin(temp_path, node->full_cmd[0]);
		free(temp_path);
		if (access(valid_path, X_OK) == 0)
		{
			node->full_path = strdup(valid_path);
			free(valid_path);
			free_array(paths);
			return ;
		}
		free(valid_path);
	}
	free_array(paths);
}

bool	check_redirections_util(t_mini *node, char **array)
{
	int	len;

	len = arraylen(array);
	if (len - 3 >= 0 && *array[len - 3] == '>' && *array[len - 2] == '>')
	{
		node->outfile = open(array[len - 1], 1, 1);
//		node->outfile = open(array[len - 1], O_CREAT | O_APPEND | O_WRONLY, 0644);
		if (node->outfile == -1)
			return (false);
	}
	else if (array && len - 2 >= 0 && *array[len - 2] == '>')
	{
		node->outfile = open(array[len - 1], 1, 1);
//		node->outfile = open(array[len - 1], O_CREAT | O_TRUNC | O_WRONLY, 0644);
		if (node->outfile == -1)
			return (false);
	}
	return (true);
}

