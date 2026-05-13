#include "../minishell.h"

static char	*get_env_value(char **envp, char *var_name)
{
	int	i;
	int	len;

	len = ft_strlen(var_name);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var_name, len) == 0 && envp[i][len] == '=')
			return (&envp[i][len + 1]);
		i++;
	}
	return (NULL);
}

static void	update_env_variable(char **envp, char *var_name, char *new_value)
{
	int		i;
	int		len;
	char	*new_entry;

	len = ft_strlen(var_name);
	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], var_name, len) == 0 && envp[i][len] == '=')
		{
			free(envp[i]);
			new_entry = ft_strjoin(var_name, "=");
			envp[i] = ft_strjoin(new_entry, new_value);
			free(new_entry);
			return ;
		}
		i++;
	}
}

static char	*get_target_dir(t_mini *mini)
{
	if (mini->full_cmd[1])
		return (mini->full_cmd[1]);
	return (get_env_value(mini->env_copy, "HOME"));
}

static void	update_pwd_variables(char *oldpwd, t_mini *mini)
{
	char	cwd[MAX_PATH];

	update_env_variable(mini->env_copy, "OLDPWD", oldpwd);
	if (getcwd(cwd, sizeof(cwd)))
		update_env_variable(mini->env_copy, "PWD", cwd);
}

int	management_cd(t_mini *mini)
{
	char	cwd[MAX_PATH];
	char	*oldpwd;
	char	*target_dir;

	oldpwd = getcwd(cwd, sizeof(cwd));
	target_dir = get_target_dir(mini);
	if (!target_dir)
	{
		perror("Error: cd");
		return (1);
	}
	if (chdir(target_dir) == -1)
	{
		perror("Error: cd");
		return (1);
	}
	update_pwd_variables(oldpwd, mini);
	return (0);
}
