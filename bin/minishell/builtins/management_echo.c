#include "../minishell.h"

int	management_echo(t_mini *mini)
{
	char	parameter;
	int		i;

	i = 1;
	parameter = 1;
	if (!mini->full_cmd[i])
		return (g_exit_status = 0);
	if (mini->full_cmd[i] && strncmp(mini->full_cmd[i], "-n", 2) == 0)
	{
		parameter = 0;
		i++;
	}
	while (mini->full_cmd[i])
	{
		write(STDOUT_FILENO, mini->full_cmd[i], strlen(mini->full_cmd[i]));
		if (mini->full_cmd[i + 1] != NULL)
			write(STDOUT_FILENO, " ", 1);
		i++;
	}
	if (parameter == 1)
		write(STDOUT_FILENO, "\n", 1);
	g_exit_status = 0;
	return (g_exit_status);
}
