#include "../include/minishell.h"

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
	t_mini	*aux;

	while (freethis)
	{
		if (freethis->full_cmd)
			ft_free_array(freethis->full_cmd);
		if (freethis->full_path)
			free(freethis->full_path);
		if (freethis->envp)
			ft_free_array(freethis->envp);
		if (freethis->env_copy)
			ft_free_array(freethis->env_copy);
		aux = freethis;
		freethis = freethis->next;
		free(aux);
	}
}

void	ft_clean_and_reset(t_mini *mini)
{
	t_mini	*aux;

	if (mini->limit)
	{
		free(mini->limit);
		mini->limit = NULL;
	}
	if (mini->next != NULL)
	{
		aux = mini->next;
		ft_free_mini(aux);
	}
	reset_mini_state(mini);
}
