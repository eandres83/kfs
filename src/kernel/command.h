#ifndef COMMAND_H
#define COMMAND_H

#include <utils.h>
#include "task/task.h"
#include "fs/ext2/ext2.h"
#include "fs/dummy.h"

enum system_state { LOGIN_MODE, PASSWD_MODE, SHELL_MODE };
extern enum system_state sys_state;

void	cat(char *path);
void	cd(char *path);
void	pwd();
bool	login(char *user_buffer, char *passwd_buffer);

#endif
