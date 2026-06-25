#include "events.h"
#include "modules/modules.h"

static struct event array_events[MAX_EVENTS][MAX_MODULES];
uint8_t active_callback_count[MAX_EVENTS] = {0};

ssize_t	register_callback(int nb_event, void *(*function)(void *data), char *module_name)
{
	if (nb_event < 0 || nb_event >= MAX_EVENTS)
		return (-1);
	for (int i = 0; i < MAX_MODULES; i++)
	{
		if (array_events[nb_event][i].name[0] == 0)
		{
			kstrcpy(array_events[nb_event][i].name, module_name);
			array_events[nb_event][i].function = function;
			active_callback_count[nb_event] += 1;
			return (0);
		}
	}
	return (-1);
}
ssize_t _api_register_callback(int nb_event, void *(*function)(void *data), char *module_name) __attribute__((alias("register_callback")));

ssize_t	desregister_callback(char *module_name)
{
	ssize_t	found = -1;
	for (int i = 0; i < MAX_EVENTS; i++)
	{
		for (int j = 0; j < MAX_MODULES; j++)
		{
			if (array_events[i][j].name[0] != 0 && kstrcmp(array_events[i][j].name, module_name) == 0)
			{
				kmemset(array_events[i][j].name, 0, 255);
				array_events[i][j].function = NULL;
				if (active_callback_count[i] > 0)
					active_callback_count[i] -= 1;
				found = 0;
			}
		}
	}
	return (found);
}
ssize_t _api_desregister_callback(char *module_name) __attribute__((alias("desregister_callback")));

void	execute_callback(int nb_event, void *data)
{
	if (active_callback_count[nb_event] == 0)
		return ;
	for (int i = 0; i < MAX_MODULES; i++)
	{
		if (array_events[nb_event][i].name[0] != 0 && array_events[nb_event][i].function != NULL)
			array_events[nb_event][i].function(data);
	}
}

