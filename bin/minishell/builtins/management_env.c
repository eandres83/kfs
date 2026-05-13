#include "../../minishell.h"

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
		write(STDOUT_FILENO, mini->env_copy[i], ft_strlen(mini->env_copy[i]));
		write(STDOUT_FILENO, "\n", 1);
		i++;
	}
	g_exit_status = 0;
	return (g_exit_status);
}
