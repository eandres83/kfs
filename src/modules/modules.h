#ifndef MODULES_H
#define MODULES_H

#include <utils.h>

#define ALIGN_PAGE(x) ((x + (PAGE_SIZE - 1)) / PAGE_SIZE)
#define MAX_MODULES 10

#define MODULE_VADDR_START 0xD0000000
#define MODULE_VADDR_END   0xD1000000

#define MOD_STATE_EMPTY		0
#define MOD_STATE_LOADING	1
#define MOD_STATE_LIVE		2
#define MOD_STATE_GOING		3

// TODO: implement linked list with head_list like linux
struct modules
{
	char		name[255];
	int 		state;

	void		*base_address;
	size_t		nb_page;

	int		(*init)();
	void		(*cleanup)();
};

// struct to create symbol_table at run time
struct symbol_table
{
	const char 	name[64];
	uint32_t	addr;
};

extern struct symbol_table array_symbols[];
extern uint32_t	size_symbols;

ssize_t	insmod(char *binary);
ssize_t	rmmod(char *module_name);

#endif
