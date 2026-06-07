#ifndef MINISHELL_H
# define MINISHELL_H

#include "../../userland/minilib.h"

#define MAX_PATH 1024

extern int	g_exit_status;

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
	ssize_t		pid;
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
int	set_env_var(t_mini *mini, const char *name, const char *value);
int	is_valid(char *str);
int	count_val(const char *val);

//execute command
void	process_command2(t_mini *mini);
void	handle_redirection1(t_mini *mini);
void	execute_one_command(t_mini *mini);
void	execute_external_command(t_mini *mini);
void	handle_redirection2(t_mini *mini);
void	reset_mini_state(t_mini *mini);
void	execute_command(t_mini *mini);

//parse and command list creation
t_mini	*process_input(t_mini *mini, char *line, char **envp);
char	**split_prompt(char const *s, char c);
char	*expand_path(char *word);
char	*expand_variable(t_mini *mini, char *word, int index);
char	*strtrim_quotes(const char *s1, int squote, int dquote);
int	count_quotes(char const *s1);
char	quote_context(const char *s, int index);
int	get_var_name_len(const char *var_name);
char	**expand(t_mini *mini, char **temp);
char	**extract_operators(char **s, const char *operators);
int	arraylen(char **array);
char	**create_temp_array(char **array, int i, int j);
char	**concatenate_array(char **array, char **temp_array, int index);
char	**final_trim(char **array);
t_mini	*initialize_mini_node(char **envp);
void	get_full_envp(t_mini *node, char **envp);
void	increment_shlvl(t_mini *mini);
bool	check_redirections(t_mini *node, char **array);
t_mini	*create_structure(t_mini *mini, char **array, char **envp);
void	get_full_command(t_mini *node, char **array);
void	check_if_builtin(t_mini *node);
void	get_path(t_mini *node);
bool	check_redirections_util(t_mini *node, char **array);

//free
void	free_array(char **array);
void	free_mini(t_mini *freethis);
void	clean_and_reset(t_mini *mini);

//other
void	error(int sig, char *str);

#endif
