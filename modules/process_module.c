#include <modules.h>

void *function_process(void *data)
{
	(void)data;
	kprintf("Another process created :)\n");
	return (NULL);
}

int init_module()
{
	if (register_callback(EVENT_PROCESS, function_process, "process_module") == -1)
		return (-1);
	kprintf("Process module init\n");
	return (0);
}

void cleanup_module()
{
	kprintf("Cleaning module :)\n");
	desregister_callback("process_module");
}

