#include "minishell.h"

static int	check_syntax(char **array)
{
	int i = 0;

	if (!array || !array[0])
		return (0);
	if (strcmp(array[0], "|") == 0)
		return (error(2, "syntax error near unexpected token '|'"), 1);
	while (array[i])
	{
		if (strcmp(array[i], "|") == 0)
		{
			if (!array[i + 1] || strcmp(array[i + 1], "|") == 0)
				return (error(2, "syntax error near unexpected token '|'"), 1);
		}
		else if (strcmp(array[i], "<") == 0 || strcmp(array[i], ">") == 0
			|| strcmp(array[i], "<<") == 0 || strcmp(array[i], ">>") == 0)
		{
			if (!array[i + 1] || strcmp(array[i + 1], "|") == 0 || strcmp(array[i + 1], "<") == 0
				|| strcmp(array[i + 1], ">") == 0 || strcmp(array[i + 1], "<<") == 0
				|| strcmp(array[i + 1], ">>") == 0)
				return (error(2, "syntax error near unexpected token"), 1);
		}
		i++;
	}
	return (0);
}

t_mini	*process_input(t_mini *mini, char *line, char **envp)
{
	char	**array;

	if (!line)
		return (NULL);
	if (count_quotes(line) == -1)
		return (error(2, "Error: Unclosed quotes"), NULL);
	array = split_prompt(line, ' ');
	if (!array || array == NULL)
	{
		error(1, "Error: Failed splitting input line");
		return (NULL);
	}
	array = expand(mini, array);
	array = extract_operators(array, "<|>");
	if (check_syntax(array) != 0)
		return (free_array(array), NULL);
	array = final_trim(array);
	mini = create_structure(mini, array, envp);
	free_array(array);
	return (mini);
}

int	g_exit_status = 0;

void	error(int sig, char *str)
{
	g_exit_status = sig;
	write(2, str, strlen(str));
	write(2, "\n", 1);
}

int	management_builtins(t_mini *mini)
{
	if (strcmp(mini->full_cmd[0], "cd") == 0)
		g_exit_status = management_cd(mini);
	else if (strcmp(mini->full_cmd[0], "pwd") == 0)
		g_exit_status = management_pwd(mini);
	else if (strcmp(mini->full_cmd[0], "env") == 0)
		g_exit_status = management_env(mini);
	else if (strcmp(mini->full_cmd[0], "unset") == 0)
		g_exit_status = management_unset(mini);
	else if (strcmp(mini->full_cmd[0], "echo") == 0)
		g_exit_status = management_echo(mini);
	else if (strcmp(mini->full_cmd[0], "export") == 0)
		g_exit_status = management_export(mini);
	else if (strcmp(mini->full_cmd[0], "exit") == 0)
		g_exit_status = management_exit(mini);
	return (g_exit_status);
}

void	get_full_envp(t_mini *mini, char **envp)
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
		mini->env_copy[i] = strdup(envp[i]);
		if (mini->env_copy[i] == NULL)
		{
			error(1, "Could not copy env variables");
			return ;
		}
		i++;
	}
	mini->env_copy[count] = NULL;
}

