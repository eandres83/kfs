#ifndef EVENTS_H
#define EVENTS_H

#include <utils.h>

#define MAX_EVENTS 30

#define EVENT_KEYBOARD	0
#define EVENT_TIMER	1
#define EVENT_PROCESS	2
#define EVENT_MEMORY	3
#define EVENT_SOCKET	4

#define EVENT_RET_TIMER 5

struct event
{
	char name[255];
	void *(*function)(void *data);
};

ssize_t	register_callback(int nb_event, void *(*function)(void *data), char *module_name);
ssize_t	desregister_callback(char *module_name);
void	execute_callback(int nb_event, void *data);

#endif
