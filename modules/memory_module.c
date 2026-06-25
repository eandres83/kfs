#include <modules.h>

void *function_memory(void *data)
{
	ssize_t	size = (ssize_t)data;
	kprintf("Size requested -> %d\n", size);
	return (NULL);
}

int init_module()
{
	if (register_callback(EVENT_MEMORY, function_memory, "memory_module") == -1)
		return (-1);
	kprintf("Memory module init\n");
	return (0);
}

void cleanup_module()
{
	kprintf("Cleanup module\n");
	desregister_callback("memory_module");
}

