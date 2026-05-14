#include "../minishell.h"

int	management_exit(t_mini *mini)
{
	int	status;

	status = g_exit_status;
	free_mini(mini);
	exit(status);
	return (status);
}

