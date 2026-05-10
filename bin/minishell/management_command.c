#include "minishell.h"

static int	management_builtins(t_mini *mini)
{
	if (ft_strcmp(mini->full_cmd[0], "cd") == 0)
		g_exit_status = management_cd(mini);
	else if (ft_strcmp(mini->full_cmd[0], "pwd") == 0)
		g_exit_status = management_pwd(mini);
	else if (ft_strcmp(mini->full_cmd[0], "env") == 0)
		g_exit_status = management_env(mini);
	else if (ft_strcmp(mini->full_cmd[0], "unset") == 0)
		g_exit_status = management_unset(mini);
	else if (ft_strcmp(mini->full_cmd[0], "echo") == 0)
		g_exit_status = management_echo(mini);
	else if (ft_strcmp(mini->full_cmd[0], "export") == 0)
		g_exit_status = management_export(mini);
	else if (ft_strcmp(mini->full_cmd[0], "exit") == 0)
		g_exit_status = management_exit(mini);
	return (g_exit_status);
}

void	execute_external_command(t_mini *mini)
{
	if (execve(mini->full_path, mini->full_cmd, mini->env_copy) == -1)
	{
		error(mini, 127, "Command not found");
		ft_free_mini(mini);
		exit(127);
	}
}

void	execute_one_command(t_mini *mini)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
	{
		error(mini, 1, "Fork error: Cannot execute child process");
		return ;
	}
	else if (pid == 0)
		execute_external_command(mini);
	else
	{
		waitpid(pid, &status, 0);
		update_exit_status(status);
	}
}

void	reset_mini_state(t_mini *mini)
{
	mini->is_builtin = 0;
	if (mini->full_path)
	{
		free(mini->full_path);
		mini->full_path = NULL;
	}
	if (mini->full_cmd)
	{
		ft_free_array(mini->full_cmd);
		mini->full_cmd = NULL;
	}
}

void	process_command2(t_mini *mini)
{
	if (mini->is_builtin)
		g_exit_status = management_builtins(mini);
	else
		execute_one_command(mini);
	reset_mini_state(mini);
}

