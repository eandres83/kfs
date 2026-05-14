#include "minishell.h"

char	**concatenate_array(char **array, char **temp_array, int index)
{
	char	**new_array;
	int		i;
	int		j;

	new_array = malloc(sizeof(char *) * (arraylen(array) + 2));
	if (!new_array)
		return (NULL);
	i = 0;
	while (i < index)
	{
		new_array[i] = strdup(array[i]);
		i++;
	}
	j = 0;
	while (temp_array[j])
		new_array[i++] = strdup(temp_array[j++]);
	while (array[index + 1])
	{
		new_array[i++] = strdup(array[index + 1]);
		index++;
	}
	new_array[i] = NULL;
	free_array(array);
	return (new_array);
}

int	arraylen(char **array)
{
	int	i = 0;

	if (!array || !array[0])
		return (0);
	while (array && array[i])
		i++;
	return (i);
}

char	**extract_operators(char **array, const char *operators)
{
	char	**temp_array;
	int	i = 0;
	int	len;

	while (array && array[i])
	{
		len = strlen(array[i]);
		if (strchr(operators, array[i][0]) && array[i][1] != '\0')
		{
			temp_array = create_temp_array(array, i, 1);
			array = concatenate_array(array, temp_array, i);
			free_array(temp_array);
		}
		else if (len > 1 && strchr(operators, array[i][len - 1])
			&& array[i][len - 2])
		{
			temp_array = create_temp_array(array, i, (len - 1));
			array = concatenate_array(array, temp_array, i);
			i++;
			free_array(temp_array);
		}
		i++;
	}
	return (array);
}

char	**create_temp_array(char **array, int i, int j)
{
	char	**temp_array;

	temp_array = malloc(sizeof(char *) * 3);
	if (!temp_array)
		return (NULL);
	temp_array[0] = substr(&array[i][0], 0, j);
	temp_array[1] = strdup(&array[i][j]);
	temp_array[2] = NULL;
	return (temp_array);
}

