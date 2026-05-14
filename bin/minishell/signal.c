#include "minishell.h"

//void	handle_sigint(int signal)
//{
//	(void)signal;
//	write(1, "\n", 1);
//	rl_on_new_line();
//	rl_replace_line("", 0);
//	rl_redisplay();
//	update_exit_status(130);
//}
// para que estoy funcione yo tengo que empaquetar en status exactamente como lo quiere las
// macros de WIFEXITED lo divide en dos partes de 8 bytes y cada uno dice una cosa pero si yo
// como kernel no se lo paso empaquetado en los 16 bytes mas bajos no sirve de nada replicar esto
//void	update_exit_status(int status)
//{
//	if (WIFEXITED(status))
//		g_exit_status = WEXITSTATUS(status);
//	else if (WIFSIGNALED(status))
//		g_exit_status = 128 + WTERMSIG(status);
//}
//
//int	get_exit_status(void)
//{
//	return (g_exit_status);
//}
//
//void	setup_signals(void)
//{
//	signal(SIGINT, handle_sigint);
//	signal(SIGQUIT, SIG_IGN);
//}

void	update_exit_status(int status)
{
	g_exit_status = status;
}

