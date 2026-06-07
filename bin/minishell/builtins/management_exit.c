#include "../minishell.h"

int	management_exit(t_mini *mini)
{
	int	status;

	status = g_exit_status;
	free_mini(mini);
	printf("Goodbye putita\n");
	exit(status);
	return (status);
}

