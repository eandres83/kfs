#include <sys/types.h>
#include "task/task.h"

uid_t setuid(uint32_t new_uid)
{
	proc_t	*process = get_current_process();
	if (new_uid > 65535)
		new_uid = 65534;
	if (process->euid == 0)
	{
		process->euid = new_uid;
		process->ruid = new_uid;
		return (0);
	}
	else if (process->ruid == new_uid)
	{
		process->euid = new_uid;
		return (0);
	}
	return (-1);
}

uid_t getuid()
{
	proc_t	*process = get_current_process();
	uint32_t uid = process->euid;
	if (uid > 65535)
		uid = 65534;
	return (uid);
}

gid_t setgid(uint32_t gid)
{
	proc_t	*process = get_current_process();
	if (gid > 65535)
		gid = 65534;
	process->rgid = gid;
	return (0);
}

gid_t	getgid()
{
	proc_t	*process = get_current_process();
	uint32_t gid = process->rgid;
	if (gid > 65535)
		gid = 65534;
	return (gid);
}

uid_t	geteuid()
{
	proc_t	*process = get_current_process();
	uint32_t uid = process->euid;
	if (uid > 65535)
		uid = 65534;
	return (uid);
}

gid_t	getegid()
{
	proc_t	*process = get_current_process();
	uint32_t gid = process->egid;
	if (gid > 65535)
		gid = 65534;
	return (gid);
}

uid_t	getuid32()
{
	proc_t	*process = get_current_process();
	return (process->euid);
}

gid_t	getgid32()
{
	proc_t	*process = get_current_process();
	return (process->rgid);
}

uid_t	setuid32(uint32_t uid)
{
	proc_t	*process = get_current_process();
	if (process->euid == 0)
	{
		process->euid = uid;
		process->ruid = uid;
		return (0);
	}
	else if (process->ruid == uid)
	{
		process->euid = uid;
		return (0);
	}
	return (-1);
}

gid_t	setgid32(uint32_t gid)
{
	proc_t	*process = get_current_process();
	process->rgid = gid;
	return (0);
}

uid_t	geteuid32()
{
	proc_t	*process = get_current_process();
	return (process->euid);
}

gid_t	getegid32()
{
	proc_t	*process = get_current_process();
	return (process->egid);
}

