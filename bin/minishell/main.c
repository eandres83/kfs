#include "minishell.h"

char	*get_name(char **env)
{
	char	*user;
	char	*prompt;
	int		i;

	i = 0;
	user = NULL;
	if (!env)
		return (ft_strdup("minishell $ "));
	while (env[i])
	{
		if (ft_strncmp("USER=", env[i], 5) == 0)
		{
			user = ft_strjoin(PURPLEB, env[i] + 5);
			break ;
		}
		i++;
	}
	if (!user)
		user = ft_strjoin(PURPLEB, "unknown");
	prompt = ft_strjoin(user, BLUEB"@minishell $ "X);
	free(user);
	return (prompt);
}

static	void	util_main(t_mini *mini, char *line, char **env)
{
	if (line[0] == ' ' || line[0] == '\0')
	{
		free(line);
		return ;
	}
	if (ft_strlen(line) > 0)
	{
		mini = ft_process_input(mini, line, env);
		add_history(line);
		if (!mini)
		{
			free(line);
//			mini = ft_initialize_mini_node(env);
			return ;
		}
		process_command2(mini);
		free(line);
	}
	ft_clean_and_reset(mini);
}

int	main(int argc, char **argv, char **env)
{
	char	*line;
	char	*name;
	t_mini	*mini;

	(void)argv;
	(void)argc;
	setup_signals();
	mini = ft_initialize_mini_node(env);
	while (1)
	{
		name = get_name(mini->env_copy);
		if (!name)
		{
			error(1, "Error: Could not get prompt name");
			break ;
		}
		line = readline(name);
		free(name);
		if (!line)
			break ;
		util_main(mini, line, env);
	}
	ft_free_mini(mini);
	rl_clear_history();
	return (0);
}

//char	*get_one_line(int fd)
//{
//	char	c;
//	char	*str;
//	char	*temp;
//	char	*aux;
//
//	str = malloc(1 * sizeof(char));
//	if (!str)
//		return (NULL);
//	str[0] = 0;
//	while (read(fd, &c, 1) > 0)
//	{
//		aux = str;
//		temp = malloc(2 * sizeof(char));
//		if (!temp)
//			return (NULL);
//		temp[0] = c;
//		temp[1] = 0;
//		str = ft_strjoin(aux, temp);
//		free(temp);
//		free(aux);
//		if (c == '\n')
//			break ;
//	}
//	return (str);
//}

