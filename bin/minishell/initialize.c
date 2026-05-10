#include "minishell.h"

static void	ft_get_full_command(t_mini *node, char **array)
{
	int	i = 0;
	int	j;
	int	k = 0;

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
		node->full_cmd[j] = ft_strdup(array[k - i]);
		k++;
	}
	node->full_cmd[j] = NULL;
}

t_mini	*ft_initialize_mini_node(char **envp)
{
	t_mini	*new_node;

	new_node = malloc(sizeof(t_mini));
	if (!new_node)
		return (NULL);
	ft_memset(new_node, 0, sizeof(t_mini));
	ft_get_full_envp(new_node, envp);
	new_node->command = NULL;
	new_node->full_cmd = NULL;
	new_node->full_path = NULL;
	return (new_node);
}

t_mini	*ft_create_structure(t_mini *mini, char **array)
{
	int	index;

	index = 0;
	ft_get_full_command(mini, &array[index]);
	ft_check_if_builtin(mini);
	ft_get_path(mini);
	return (mini);
}

void	ft_free_array(char **array)
{
	int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		array[i] = NULL;
		i++;
	}
	free(array);
	array = NULL;
}

void	ft_free_mini(t_mini *freethis)
{
	if (freethis->full_cmd)
		ft_free_array(freethis->full_cmd);
	if (freethis->full_path)
		free(freethis->full_path);
	if (freethis->envp)
		ft_free_array(freethis->envp);
	if (freethis->env_copy)
		ft_free_array(freethis->env_copy);
	free(freethis);
}

