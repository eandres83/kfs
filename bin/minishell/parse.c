#include "../minishell.h"

static int	ft_check_syntax(char **array)
{
	int i = 0;

	if (!array || !array[0])
		return (0);
	if (ft_strcmp(array[0], "|") == 0)
		return (error(2, "syntax error near unexpected token '|'"), 1);
	while (array[i])
	{
		if (ft_strcmp(array[i], "|") == 0)
		{
			if (!array[i + 1] || ft_strcmp(array[i + 1], "|") == 0)
				return (error(2, "syntax error near unexpected token '|'"), 1);
		}
		else if (ft_strcmp(array[i], "<") == 0 || ft_strcmp(array[i], ">") == 0
			|| ft_strcmp(array[i], "<<") == 0 || ft_strcmp(array[i], ">>") == 0)
		{
			if (!array[i + 1] || ft_strcmp(array[i + 1], "|") == 0 || ft_strcmp(array[i + 1], "<") == 0
				|| ft_strcmp(array[i + 1], ">") == 0 || ft_strcmp(array[i + 1], "<<") == 0
				|| ft_strcmp(array[i + 1], ">>") == 0)
				return (error(2, "syntax error near unexpected token"), 1);
		}
		i++;
	}
	return (0);
}

t_mini	*ft_process_input(t_mini *mini, char *line, char **envp)
{
	char	**array;

	if (!line)
		return (NULL);
	if (ft_count_quotes(line) == -1)
		return (error(2, "Error: Unclosed quotes"), NULL);
	array = ft_split_prompt(line, ' ');
	if (!array || array == NULL)
	{
		error(1, "Error: Failed splitting input line");
		return (NULL);
	}
	array = ft_expand(mini, array);
	array = ft_extract_operators(array, "<|>");
	if (ft_check_syntax(array) != 0)
		return (ft_free_array(array), NULL);
	array = ft_final_trim(array);
	mini = ft_create_structure(mini, array, envp);
	ft_free_array(array);
	return (mini);
}

int	g_exit_status = 0;

void	error(int sig, char *str)
{
	g_exit_status = sig;
	ft_putstr_fd(str, 2);
	ft_putstr_fd("\n", 2);
}

int	management_builtins(t_mini *mini)
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

void	ft_get_full_envp(t_mini *mini, char **envp)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (envp[count] != NULL)
		count++;
	mini->env_copy = malloc((count + 1) * sizeof(char *));
	if (mini->env_copy == NULL)
		return ;
	while (i < count)
	{
		mini->env_copy[i] = ft_strdup(envp[i]);
		if (mini->env_copy[i] == NULL)
		{
			error(1, "Could not copy env variables");
			return ;
		}
		i++;
	}
	mini->env_copy[count] = NULL;
}

