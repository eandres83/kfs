#include "minishell.h"

void	ft_handle_sigint(int signal)
{
	(void)signal;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	update_exit_status(130);
}

void	update_exit_status(int status)
{
	if (WIFEXITED(status))
		g_exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		g_exit_status = 128 + WTERMSIG(status);
}

int	get_exit_status(void)
{
	return (g_exit_status);
}

void	setup_signals(void)
{
	signal(SIGINT, ft_handle_sigint);
	signal(SIGQUIT, SIG_IGN);
}
