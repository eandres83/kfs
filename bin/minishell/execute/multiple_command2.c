#include "../minishell.h"

void	execute_command(t_mini *mini)
{
	if (mini->is_builtin == 1)
	{
		management_builtins(mini);
		exit(1);
	}
	else
	{
		execute_external_command(mini);
		exit(0);
	}
}

