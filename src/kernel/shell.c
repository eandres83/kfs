#include <utils.h>
#include <kmalloc.h>
#include "drivers/keyboard.h"
#include "drivers/vga.h"
#include "../drivers/io.h"
#include "mm/gdt.h"
#include "mm/pmm.h"
#include "mm/vmm.h"
#include "fs/vfs/vfs.h"
#include "kernel/command.h"

#define BUFFER_SIZE 256

static char buffer[BUFFER_SIZE];
static char user_buffer[BUFFER_SIZE];
static char passwd_buffer[BUFFER_SIZE];
static uint32_t index = 0;

static void reboot()
{
	uint8_t good = 0x02;
	while (good & 0x02)
		good = inb(0x64);
	outb(0x64, 0xFE);
	asm volatile("hlt");
}

static void	execute_command(char *str)
{
	if (kstrcmp(str, "reboot") == 0)
		reboot();
	else if (kstrcmp(str, "shutdown") == 0)
		outw(0x604, 0x2000);
	else if (kstrcmp(str, "clear") == 0)
		terminal_initialize();
	else if (kstrcmp(str, "meminfo") == 0)
		meminfo();
	else if (kstrcmp(str, "layout fr") == 0)
		set_keyboard_layout("fr");
	else if (kstrcmp(str, "layout us") == 0)
		set_keyboard_layout("us");
	else if (kstrncmp(str, "cat", 3) == 0)
	{
		char *path = str + 4;
		cat(path);
	}
	else if (kstrncmp(str, "ls", 2) == 0)
	{
		char *path = str + 3;
		if (path[0] != '\0')
			ls(path);
		else
			ls(NULL);
	}
	else if (kstrcmp(str, "pwd") == 0)
		pwd();
	else if (kstrncmp(str, "cd", 2) == 0)
	{
		char *path = str + 3;
		cd(path);
	}
	else if (kstrncmp(str, "create", 6) == 0)
	{
		str[kstrlen(str)] = '\n';
		char *file_name = str + 7;

		int i;
		char name[124] = {0};
		for (i = 0; file_name[i] != ' ' && file_name[i] != '\0'; i++)
			name[i] = file_name[i];
		char *content = file_name + i + 1;

		struct vfs_node *node = get_current_node();
		if (node->ops && node->ops->write)
		{
			size_t ret = node->ops->write(get_current_node(), content, name);
			if (ret == 0)
				kprintf("Error: cannot write :(\n");
		}
		else
			kprintf("PUTADON BRO\n");
	}
	else if (kstrncmp(str, "mount", 5) == 0)
	{
		char *path = str + 6;

		char partition_nb[124] = {0};
		int i;
		for (i = 0; path[i] != ' ' && path[i] != '\0'; i++)
			partition_nb[i] = path[i];
		size_t partition = katoi(partition_nb);
		i++;
		if (path[i] == '\0')
		{
			kprintf("Error: wrong argument in mount command :(\n");
			return ;
		}
		char name[124] = {0};
		for (int j = 0; path[i] != '\0'; i++, j++)
			name[j] = path[i];
		mount(name, partition);
	}
	else if (kstrncmp(str, "umount", 6) == 0)
	{
		char *path = str + 7;
		umount(path);
	}
	else if (kstrlen(str) > 0)
		kprintf("Unknown command: %s\n", str);
}

// machine state done, for manage login
void	shell_handle_keypress(char c)
{
	terminal_setcolor(VGA_COLOR_LIGHT_CYAN);

	if (c > 0)
	{
		if (c == '\n')
		{
			terminal_putchar('\n');
			if (sys_state == LOGIN_MODE)
			{
				user_buffer[index] = '\0';
				sys_state = PASSWD_MODE;
				kprintf("PASSWORD> ");
			}
			else if (sys_state == PASSWD_MODE)
			{
				passwd_buffer[index] = '\0';
				if (!(login(user_buffer, passwd_buffer)))
				{
					kmemset(user_buffer, 0, BUFFER_SIZE);
					kmemset(passwd_buffer, 0, BUFFER_SIZE);
					sys_state = LOGIN_MODE;
					kprintf("LOGIN> ");
				}
				else
				{
					sys_state = SHELL_MODE;
					kprintf("KFS> ");
				}
			}
			else
			{
				buffer[index] = '\0';
				execute_command(buffer);
				kmemset(buffer, 0, BUFFER_SIZE);
				kprintf("KFS> ");
			}
			index = 0;
		}
		else if (c == '\b')
		{
			if (index > 0)
			{
				index--;
				terminal_putchar('\b');
				terminal_putchar(' ');
				terminal_putchar('\b');
			}
		}
		else
		{
			if (index < BUFFER_SIZE - 1)
			{
				if (sys_state == LOGIN_MODE)
					user_buffer[index] = c;
				else if (sys_state == PASSWD_MODE)
				{
					passwd_buffer[index] = c;
					terminal_putchar('*');
					index++;
					return ;
				}
				else
					buffer[index] = c;
				terminal_putchar(c);
				index++;
			}
		}
	}
}

