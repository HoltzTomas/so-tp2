#include "commands.h"
#include <stdio.h>

extern void sys_clear(uint32_t color);

static int clear_func(int argc, char **argv) {
	sys_clear(0);
	return 0;
}

command clear_cmd = {"clear", clear_func, "Clear screen"};
