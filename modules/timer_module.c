#include <modules.h>

const char *str = "HOLA";

void check_section_attributes()
{
	char *addr = (char*)str;
	*addr = 'a';
}

int var = 0;

void *return_value(void *data)
{
	*(int*)data = var;
	return (NULL);
}

void *function_timer(void *data)
{
	(void)data;
//	uint32_t num = (uint32_t)data;
	var++;
	return (NULL);
}

int init_module()
{
	if (register_callback(EVENT_TIMER, function_timer, "timer_module") == -1)
		return (-1);
	if (register_callback(EVENT_RET_TIMER, return_value, "ret_timer_module") == -1)
		return (-1);
	kprintf("Module cargado\n");
	return (0);
}

void cleanup_module()
{
	kprintf("Cleaning module :)\n");
	kprintf("El valor del var -> %d\n", var);
	desregister_callback("timer_module");
	desregister_callback("ret_timer_module");
}

