void exit(uint32_t status)
{
	asm volatile("int $0x80" : : "a" (1), "b" (status) : "memory");
	while (1);
}

ssize_t fork()
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (2) : "memory");
	return (ret);
}

ssize_t write(char *str, size_t len)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (4), "b" (1), "c" (str), "d" (len) : "memory");
	return (ret);
}

ssize_t wait(uint32_t *status)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (7), "b" (status) : "memory");
	return (ret);
}

ssize_t execve(char *file_path, char **argv, char **envp)
{
	ssize_t	ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (11), "b" (file_path), "c" (argv), "d" (envp) : "memory");
	return (ret);
}

ssize_t kill(uint32_t pid, uint32_t sig)
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (37), "b" (pid), "c" (sig) : "memory");
	return (ret);
}

ssize_t signal(uint32_t num, void (*function)())
{
	ssize_t ret;
	asm volatile("int $0x80" : "=a" (ret) : "a" (48), "b" (num), "c" (function) : "memory");
	return (ret);
}

