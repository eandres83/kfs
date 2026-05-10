#include "../../include/minishell.h"

int	management_env(t_mini *mini)
{
	int	i;

	i = 0;
	if (!mini->env_copy)
	{
		g_exit_status = 1;
		return (g_exit_status);
	}
	while (mini->env_copy[i])
	{
		write(1, mini->env_copy[i], ft_strlen(mini->env_copy[i]));
		write(1, "\n", 1);
		i++;
	}
	g_exit_status = 0;
	return (g_exit_status);
}
