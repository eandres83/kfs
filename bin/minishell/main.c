#include "minishell.h"

char	*get_name(char **env)
{
	char	*user;
	char	*prompt;
	int		i;

	i = 0;
	user = NULL;
	if (!env)
		return (strdup("minishell $ "));
	while (env[i])
	{
		if (strncmp("USER=", env[i], 5) == 0)
		{
			user = strdup(env[i] + 5);
			break ;
		}
		i++;
	}
	if (!user)
		user = strdup("unknown");
	prompt = strjoin(user, "@minishell $ ");
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
	if (strlen(line) > 0)
	{
		mini = process_input(mini, line, env);
		if (!mini)
		{
			free(line);
			return ;
		}
		process_command2(mini);
		free(line);
	}
	clean_and_reset(mini);
}

static char	*get_one_line(int fd)
{
	char	c;
	char	*str;
	char	*temp;
	char	*aux;

	str = malloc(1 * sizeof(char));
	if (!str)
		return (NULL);
	str[0] = 0;
	int ret;
	while ((ret = read(fd, &c, 1)) > 0)
	{
		if (c == '\n')
			break ;
		aux = str;
		temp = malloc(2 * sizeof(char));
		if (!temp)
			return (NULL);
		temp[0] = c;
		temp[1] = 0;
		str = strjoin(aux, temp);
		free(temp);
		free(aux);
	}
	return (str);
}

int	main(int argc, char **argv, char **env)
{
	char	*line;
	char	*name;
	t_mini	*mini;

	(void)argv;
	(void)argc;
//	setup_signals();
	printf("Estoy en mi puta minishell desde ring3\n");
	mini = initialize_mini_node(env);
	while (1)
	{
		name = get_name(mini->env_copy);
		if (!name)
		{
			error(1, "Error: Could not get prompt name");
			break ;
		}
		printf("%s ", name);
		line = get_one_line(0);
		free(name);
		if (!line)
			break ;
		util_main(mini, line, env);
	}
	free_mini(mini);
	return (0);
}

