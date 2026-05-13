#include "../minishell.h"

int	create_pipes(int pipefd[2])
{
	if (pipe(pipefd) == -1)
	{
		perror("Could not create pipe");
		return (EXIT_FAILURE);
	}
	return (0);
}

void	pipe_output(int pipefd[2])
{
	close(pipefd[0]);
	if (dup2(pipefd[1], STDOUT_FILENO) == -1)
	{
		perror("Dup2 error: Output redirection failed");
		exit(9);
	}
	close(pipefd[1]);
}

void	pipe_input(int last_fd)
{
	if (last_fd != STDIN_FILENO)
	{
		if (dup2(last_fd, STDIN_FILENO) == -1)
		{
			perror("Dup2 error: Input redirection failed");
			exit(9);
		}
		close(last_fd);
	}
}

void	close_pipe(int pipefd[2], int last_fd)
{
	if (last_fd != STDIN_FILENO)
		close(last_fd);
	if (pipefd[1] != STDOUT_FILENO)
		close(pipefd[1]);
}

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
