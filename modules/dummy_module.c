#include <modules.h>

const char *str = "HOLA";

void check_section_attributes()
{
	char *addr = (char*)str;
	*addr = 'a';
}

int init_module()
{
	int res = kstrcmp("1", "1");
	if (res == 0)
		kprintf("sssssssssssssssssssssssssssssssssssssss\n");
	return (0);
}

void cleanup_module()
{
	kprintf("Cleaning module :)\n");
}

