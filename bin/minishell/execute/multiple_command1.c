#include "../minishell.h"

void	h_m_c(int pipefd[2], int last_fd, t_mini *mini, t_mini *next_cmd)
{
	ssize_t	pid;

	pid = fork();
	if (pid == -1)
	{
		error(1, "fork");
		exit(EXIT_FAILURE);
	}
	if (pid == 0)
	{
		pipe_input(last_fd);
		if (next_cmd)
			pipe_output(pipefd);
		handle_redirection1(mini);
		handle_redirection2(mini);
		if (mini->is_builtin)
			management_builtins(mini);
		else
			execute_external_command(mini);
		exit(EXIT_SUCCESS);
	}
	mini->pid = pid;
}

void	execute_multiples_command(t_mini *mini)
{
	int	last_fd;
	int	pipefd[2];
	int	status;
	t_mini	*current;

	last_fd = STDIN_FILENO;
	current = mini;
	while (current)
	{
		if (current->next)
		{
			waitpid(mini->pid, &status, 0);
			if (create_pipes(pipefd) == -1)
				return ;
		}
		h_m_c(pipefd, last_fd, current, current->next);
		close_pipe(pipefd, last_fd);
		if (current->next)
			last_fd = pipefd[0];
		current = current->next;
	}
	while (wait(&status) > 0)
		update_exit_status(status);
}

void	reset_mini_state(t_mini *mini)
{
	mini->is_builtin = 0;
	mini->next = NULL;
	if (mini->infile != STDIN_FILENO)
	{
		close(mini->infile);
		mini->infile = STDIN_FILENO;
	}
	if (mini->outfile != STDOUT_FILENO)
	{
		close(mini->outfile);
		mini->outfile = STDOUT_FILENO;
	}
	if (mini->full_path)
	{
		free(mini->full_path);
		mini->full_path = NULL;
	}
	if (mini->full_cmd)
	{
		free_array(mini->full_cmd);
		mini->full_cmd = NULL;
	}
}

void	process_command2(t_mini *mini)
{
	int origin_stdin = 0;
        int origin_stdout = 0;
	if (mini->next == NULL && mini->is_builtin)
	{
		// save father FDs
		origin_stdin = dup(STDIN_FILENO);
		origin_stdout = dup(STDOUT_FILENO);
		handle_redirection1(mini);
		handle_redirection2(mini);
		management_builtins(mini);
		dup2(origin_stdin, STDIN_FILENO);
		dup2(origin_stdout, STDOUT_FILENO);
		close(origin_stdin);
		close(origin_stdout);
	}
	else if (mini->next == NULL)
		execute_one_command(mini);
	else
		execute_multiples_command(mini);
	reset_mini_state(mini);
}

