#include "../userland/minilib.h"

// test timer
int main()
{
//	char *str = (char*)0xFFFFF000;
	char *str1 = "/bin/timer_module.ko";
	ssize_t res = init_module(str1);
	if (res == -1)
		return (printf("Miredon el modulo broo; res -> %d\n", res), -1);
	for (volatile int i = 0; i < 100000000; i++);
	
	ssize_t	res1 = del_module("timer_module.ko");
	if (res1 == -1)
		return (printf("Otro mierdon el del module acho; res -> %d\n", res1), -1);
//	ssize_t res2 = init_module("/bin/dummy_module.ko");
//	if (res2 == -1)
//		return (printf("Miredon el modulo broo; res -> %d\n", res2), -1);
	while (1);
}

// test keyboard
//int main()
//{
//	ssize_t	res = init_module("/bin/keyboard_module.ko");
//	if (res == -1)
//		return (printf("Mierdon de moduleo bro\n"), -1);
////	ssize_t	res1 = del_module("keyboard_module.ko");
////	if (res1 == -1)
////		return (printf("Otra mierda del\n"), -1);
//	
//	while(1);
//}

// test process
//int main()
//{
//	ssize_t	res = init_module("/bin/process_module.ko");
//	if (res == -1)
//		return (printf("Miredon el modulo broo; res -> %d\n", res), -1);
//	for (volatile int i = 0; i < 1000000; i++);
//
//	ssize_t pid = fork();
//	if (pid == 0)
//	{
//		ssize_t res = del_module("process_module.ko");
//		if (res == -1)
//			return (printf("Otro mierdon en del module acho;\n"), res);
//	}
//	while (1);
//}

// test socket
//int main()
//{
//	ssize_t res = init_module("/bin/socket_module.ko");
//	if (res == -1)
//		return (printf("MIerdon el modulo bro\n"), res);
//
//	int socket[2];
//	ssize_t res_socket = socketpair(AF_UNIX, SOCK_STREAM, 0, socket);
//	if (res_socket == -1)
//		return (printf("Puto erro en socketpair :(\n"), -1);
//
//	ssize_t res1 = del_module("socket_module.ko");
//	if (res1 == -1)
//		return (printf("Otro mierdon en del module acho;\n"), res);
//	while(1);
//}

// test memory
//int main()
//{
//	ssize_t res = init_module("/bin/memory_module.ko");
//	if (res == -1)
//		return (printf("Mierdon el modulo bro\n"), res);
//
//	void *addr = malloc(4097);
//	void *addr1 = malloc(4097);
//	void *addr2 = malloc(4097);
//	if (!addr || !addr1 || !addr2)
//		return (printf("el puto malloc otra vez\n"), -1);
//	ssize_t	res1 = del_module("memory_module.ko");
//	if (res1 == -1)
//		return (printf("Otro mierdon en del module acho;\n"), res);
//	while (1);
//}

