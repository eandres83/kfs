#include "../minishell.h"

void	handle_redirection1(t_mini *mini)
{
	if (mini->infile != STDIN_FILENO)
	{
		if (dup2(mini->infile, STDIN_FILENO) == -1)
		{
			error(9, "Dup2 error: Input redirection failed");
			exit(9);
		}
		close(mini->infile);
	}
}

void	handle_redirection2(t_mini *mini)
{
	if (mini->outfile != STDOUT_FILENO)
	{
		if (dup2(mini->outfile, STDOUT_FILENO) == -1)
		{
			error(1, "Dup2 error: Output redirection failed");
			exit(9);
		}
		close(mini->outfile);
	}
}

void	execute_external_command(t_mini *mini)
{
	if (!mini->full_cmd && !mini->full_cmd[0])
		exit(1);
	if (execve(mini->full_path, mini->full_cmd, mini->env_copy) == -1)
	{
		error(127, "Command not found");
		exit(127);
	}
}

void	execute_one_command(t_mini *mini)
{
	ssize_t	pid;
	int	status;

	pid = fork();
	if (pid == -1)
	{
		error(1, "Fork error: Cannot execute child process");
		return ;
	}
	else if (pid == 0)
	{
		handle_redirection1(mini);
		handle_redirection2(mini);
		execute_external_command(mini);
	}
	else
	{
		if (mini->infile != STDIN_FILENO)
			close(mini->infile);
		if (mini->outfile != STDOUT_FILENO)
			close(mini->outfile);
//		waitpid(pid, &status, 0);
		wait(&status);
		update_exit_status(status);
	}
}
