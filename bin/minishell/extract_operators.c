#include "../minishell.h"

char	**ft_concatenate_array(char **array, char **temp_array, int index)
{
	char	**new_array;
	int		i;
	int		j;

	new_array = malloc(sizeof(char *) * (ft_arraylen(array) + 2));
	if (!new_array)
		return (NULL);
	i = 0;
	while (i < index)
	{
		new_array[i] = ft_strdup(array[i]);
		i++;
	}
	j = 0;
	while (temp_array[j])
		new_array[i++] = ft_strdup(temp_array[j++]);
	while (array[index + 1])
	{
		new_array[i++] = ft_strdup(array[index + 1]);
		index++;
	}
	new_array[i] = NULL;
	ft_free_array(array);
	return (new_array);
}

int	ft_arraylen(char **array)
{
	int	i = 0;

	if (!array || !array[0])
		return (0);
	while (array && array[i])
		i++;
	return (i);
}

char	**ft_extract_operators(char **array, const char *operators)
{
	char	**temp_array;
	int	i = 0;
	int	len;

	while (array && array[i])
	{
		len = ft_strlen(array[i]);
		if (ft_strchr(operators, array[i][0]) && array[i][1] != '\0')
		{
			temp_array = ft_create_temp_array(array, i, 1);
			array = ft_concatenate_array(array, temp_array, i);
			ft_free_array(temp_array);
		}
		else if (len > 1 && ft_strchr(operators, array[i][len - 1])
			&& array[i][len - 2])
		{
			temp_array = ft_create_temp_array(array, i, (len - 1));
			array = ft_concatenate_array(array, temp_array, i);
			i++;
			ft_free_array(temp_array);
		}
		i++;
	}
	return (array);
}

char	**ft_create_temp_array(char **array, int i, int j)
{
	char	**temp_array;

	temp_array = malloc(sizeof(char *) * 3);
	if (!temp_array)
		return (NULL);
	temp_array[0] = ft_substr(&array[i][0], 0, j);
	temp_array[1] = ft_strdup(&array[i][j]);
	temp_array[2] = NULL;
	return (temp_array);
}

