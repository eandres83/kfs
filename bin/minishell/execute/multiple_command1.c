#include "../minishell.h"

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
	reset_mini_state(mini);
}

