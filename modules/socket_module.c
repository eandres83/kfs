#include <modules.h>

void *function_socket(void *data)
{
	kprintf("Socket created\n");
	int *fds = (int *)data;
	kprintf("fds -> %d, %d\n", fds[0], fds[1]);
	return (NULL);
}

int init_module()
{
	if (register_callback(EVENT_SOCKET, function_socket, "socket_module") == -1)
		return (-1);
	kprintf("Socket module init\n");
	return (0);
}

void cleanup_module()
{
	kprintf("Cleanup module :)\n");
	desregister_callback("socket_module");
}

