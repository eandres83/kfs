#include "../userland/minilib.h"
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
		write(fd, &c, 1);
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

static char **read_split_file(char *file)
{
	ssize_t fd = open(file, O_RDONLY, 1);
	if (fd < 0)
	{
		printf("Error: cannot read %s\n", file);
		return (NULL);
	}
	char *buff = (char*)malloc(4097);
	if (!buff)
		return (NULL);
	ssize_t res = read(fd, buff, 4096);
	if (res == -1)
		return (free(buff), NULL);
	buff[res] = '\0';
	char **split_buf = split(buff, '\n');
	if (!split_buf)
		return (free(buff), NULL);
	free(buff);
	return (split_buf);
}

static int check_passwd(char **split_shadow, char *passwd, char *user)
{
	for (int i = 0; split_shadow[i] != NULL; i++)
	{
		char **array = split(split_shadow[i], ':');
		if (!array)
			return (1);
		if (strcmp(user, array[0]) == 0)
		{
			if (strcmp(passwd, array[1]) == 0)
				return (0);
		}
		dfree(array);
	}
	return (1);
}

static char **manage_login()
{
	char **split_passwd = read_split_file("/etc/passwd");
	if (!split_passwd)
		return (NULL);
	char **split_shadow = read_split_file("/etc/shadow");
	if (!split_shadow)
		return (dfree(split_passwd), NULL);

	while (1)
	{
		printf("LOGIN> ");
		char *user = get_one_line(0);
		printf("PASSWD> ");
		char *passwd = get_one_line(0);

		for (int i = 0; split_passwd[i] != NULL; i++)
		{
			char **split_user = split(split_passwd[i], ':');
			if (!split_user)
				return (free(user), free(passwd), dfree(split_passwd), dfree(split_shadow), NULL);
			if (strcmp(split_user[0], user) == 0)
			{
				if (check_passwd(split_shadow, passwd, split_user[0]) == 0)
					return (free(user), free(passwd), dfree(split_passwd), dfree(split_shadow), split_user);
			}
			dfree(split_user);
		}
		free(user);
		free(passwd);
		printf("Error: bad user or password :(\n");
	}
	dfree(split_passwd);
	dfree(split_shadow);
	return (NULL);
}

int main()
{
	char **line = manage_login();
	if (line == NULL)
		return (printf("Error: something bad in login\n"), 1);

	char *argv[] = { "/bin/minishell", NULL };
	char *user = strjoin("USER=", line[0]);
	char *directory = strjoin("HOME=", line[5]);
	char *pwd = strjoin("PWD=", line[5]);
	char *shell = strjoin("SHELL=", line[6]);
	char *envp[] = { "PATH=/usr/local/bin:/bin:/usr/bin", "TERM=linux", user, directory, shell, pwd, NULL };

	ssize_t pid = fork();
	if (pid == 0)
	{
		if (chdir(line[5]) == -1)
		{
			printf("Fatal error in chdir syscall\n");
			exit(1);
		}
		ssize_t res = execve("/bin/test_pipe", argv, envp);
		if (res == -1)
		{
			printf("Fatal error in init process execve\n");
			exit(1);
		}
	}
	else if (pid > 0)
	{
		int status = 0;
		waitpid(pid, &status, 0);
		if (status == 1)
			printf("Fuera de mi puta shell bro\n");
	}
	dfree(line);
	return (0);
}

