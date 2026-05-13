#include "../minishell.h"

int	management_exit(t_mini *mini)
{
	int	status;

	status = g_exit_status;
	ft_free_mini(mini);
	rl_clear_history();
	exit(status);
}
