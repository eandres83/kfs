#include <modules.h>

void *function_keyboard(void *data)
{
	uint8_t val = *(uint8_t*)data;
	kprintf("Pressed -> %d\n", val);
	return (NULL);
}

int init_module()
{
	if (register_callback(EVENT_KEYBOARD, function_keyboard, "keyboard_module") == -1)
		return (-1);
	kprintf("Init module\n");
	return (0);
}

void cleanup_module()
{
	kprintf("Cleaning module :)\n");
	desregister_callback("keyboard_module");
}

