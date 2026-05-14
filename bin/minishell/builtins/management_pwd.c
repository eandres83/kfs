#include "../minishell.h"

int	management_pwd(t_mini *mini)
{
	char	cwd[MAX_PATH];

	(void)mini;
	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		write(STDOUT_FILENO, cwd, strlen(cwd));
		write(STDOUT_FILENO, "\n", 1);
		g_exit_status = 0;
	}
	else
	{
		error(1, "Error: pwd");
		g_exit_status = 1;
	}
	return (g_exit_status);
}

void	update_pwd(t_mini *mini)
{
	char	new_path[MAX_PATH];

	if (getcwd(new_path, sizeof(new_path)) != NULL)
	{
		set_env_var(mini, "OLDPWD", mini->full_path);
		set_env_var(mini, "PWD", new_path);
		free(mini->full_path);
		mini->full_path = strdup(new_path);
	}
	else
		error(1, "Error: pwd");
}
