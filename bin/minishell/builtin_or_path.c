#include "minishell.h"

int	g_exit_status = 0;

void	error(t_mini *mini, int sig, char *str)
{
	(void)mini;
	g_exit_status = sig;
	ft_putstr_fd(str, 2);
	ft_putstr_fd("\n", 2);
}

void	ft_check_if_builtin(t_mini *node)
{
	if (!ft_strcmp(node->full_cmd[0], "echo"))
		node->is_builtin = 1;
	else if (!ft_strcmp(node->full_cmd[0], "cd"))
		node->is_builtin = 1;
	else if (!ft_strcmp(node->full_cmd[0], "pwd"))
		node->is_builtin = 1;
	else if (!ft_strcmp(node->full_cmd[0], "export"))
		node->is_builtin = 1;
	else if (!ft_strcmp(node->full_cmd[0], "unset"))
		node->is_builtin = 1;
	else if (!ft_strcmp(node->full_cmd[0], "env"))
		node->is_builtin = 1;
	else if (!ft_strcmp(node->full_cmd[0], "exit"))
		node->is_builtin = 1;
}

static void	check_this(t_mini *node)
{
	if (node->full_cmd[0] && (node->full_cmd[0][0] == '/'
		|| node->full_cmd[0][0] == '.'))
	{
		node->full_path = ft_strdup(node->full_cmd[0]);
		return ;
	}
}

static	char	**ft_get_path_util(t_mini *node, char **paths)
{
	int	i;

	check_this(node);
	i = 0;
	while (node->env_copy[i])
	{
		if (ft_strncmp("PATH=", node->env_copy[i], 5) == 0)
		{
			paths = ft_split(&(node->env_copy[i][5]), ':');
			break ;
		}
		else
			i++;
	}
	if (!paths)
		return (NULL);
	return (paths);
}

void	ft_get_path(t_mini *node)
{
	char	**paths;
	char	*temp_path;
	char	*valid_path;
	int		i;

	if (node->is_builtin == 1)
		return ;
	paths = NULL;
	paths = ft_get_path_util(node, paths);
	i = 0;
	while (paths && paths[i])
	{
		temp_path = ft_strjoin(paths[i++], "/");
		valid_path = ft_strjoin(temp_path, node->full_cmd[0]);
		free(temp_path);
		if (access(valid_path, X_OK) == 0)
		{
			node->full_path = ft_strdup(valid_path);
			free(valid_path);
			ft_free_array(paths);
			return ;
		}
		free(valid_path);
	}
	ft_free_array(paths);
}

