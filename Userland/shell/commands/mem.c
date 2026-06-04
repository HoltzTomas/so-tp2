#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

static int mem_func(int argc, char **argv) {
	uint64_t total = 0, free_mem = 0;
	sys_mem_info(&total, &free_mem);
	printf("Total: %d bytes\n", (int)total);
	printf("Used:  %d bytes\n", (int)(total - free_mem));
	printf("Free:  %d bytes\n", (int)free_mem);
	return 0;
}

command mem_cmd = {"mem", mem_func, "Show memory info"};
