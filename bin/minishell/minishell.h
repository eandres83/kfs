#ifndef MINISHELL_H
# define MINISHELL_H

#include "../../userland/minilib.h"

# define MAX_PATH 1024

extern int	g_exit_status;

# define RED		"\033[0;31m"
# define GREEN		"\033[0;32m"
# define GREENB		"\033[32;1m"
# define YELLOW		"\033[0;33m"
# define BLUE 		"\033[0;34m"
# define BLUEB 		"\033[34;1m"
# define PURPLE		"\033[0;35m"
# define PURPLEB	"\033[35;1m"
# define CYAN		"\033[0;36m"
# define BOLD		"\033[0;1m"
# define X		"\033[0;0m"

typedef struct s_mini
{
	char		*command;
	char		**full_cmd;
	char		*full_path;
	char		**envp;
	char		**env_copy;
	int		is_builtin;
	int		num;
	int		infile;
	int		outfile;
	pid_t		pid;
	char		*limit;
	struct s_mini	*next;
}t_mini;

// builtins
int		management_builtins(t_mini *mini);
int		management_pwd(t_mini *mini);
int		management_cd(t_mini *mini);
int		management_env(t_mini *mini);
int		management_unset(t_mini *mini);
int		management_echo(t_mini *mini);
int		management_export(t_mini *mini);
int		management_exit(t_mini *mini);

//builtins utils
void	update_pwd(t_mini *mini);
char	**create_env_copy(char **env);
int		set_env_var(t_mini *mini, const char *name, const char *value);
int		is_valid(char *str);
int		count_val(const char *val);

//execute command
void	process_command2(t_mini *mini);
void	handle_redirection1(t_mini *mini);
void	execute_one_command(t_mini *mini);
int		create_pipes(int pipefd[2]);
void	execute_external_command(t_mini *mini);
void	h_m_c(int pipefd[2], int last_fd, t_mini *mini, t_mini *next_cmd);
void	close_pipe(int pipefd[2], int last_fd);
void	pipe_output(int pipefd[2]);
void	pipe_input(int last_fd);
void	handle_redirection2(t_mini *mini);
void	reset_mini_state(t_mini *mini);
void	execute_command(t_mini *mini);

//parse and command list creation
t_mini	*ft_process_input(t_mini *mini, char *line, char **envp);
char	**ft_split_prompt(char const *s, char c);
char	*ft_expand_path(char *word);
char	*ft_expand_variable(t_mini *mini, char *word, int index);
char	*ft_strtrim_quotes(const char *s1, int squote, int dquote);
int		ft_count_quotes(char const *s1);
char	ft_quote_context(const char *s, int index);
int		ft_get_var_name_len(const char *var_name);
char	**ft_expand(t_mini *mini, char **temp);
char	**ft_extract_operators(char **s, const char *operators);
int		ft_arraylen(char **array);
char	**ft_create_temp_array(char **array, int i, int j);
char	**ft_concatenate_array(char **array, char **temp_array, int index);
char	**ft_final_trim(char **array);
t_mini	*ft_initialize_mini_node(char **envp);
void	ft_get_full_envp(t_mini *node, char **envp);
void	ft_increment_shlvl(t_mini *mini);
bool	ft_check_redirections(t_mini *node, char **array);
t_mini	*ft_create_structure(t_mini *mini, char **array, char **envp);
void	ft_get_full_command(t_mini *node, char **array);
int		ft_locate_pipe(char **array, int *index);
void	ft_check_if_builtin(t_mini *node);
void	ft_get_path(t_mini *node);
bool	ft_check_redirections_util(t_mini *node, char **array);

//free
void	ft_free_array(char **array);
void	ft_free_mini(t_mini *freethis);
void	ft_clean_and_reset(t_mini *mini);

//signal
void	ft_handle_sigint(int signal);
void	update_exit_status(int status);
int		get_exit_status(void);

//other
void	setup_signals(void);
void	error(int sig, char *str);

#endif
