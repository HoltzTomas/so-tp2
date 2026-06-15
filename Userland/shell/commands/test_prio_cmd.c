#include "commands.h"
#include <stdio.h>

extern int test_prio(int argc, char *argv[]);

static int test_prio_func(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: test_prio <max_value>\n");
		printf("  max_value: each process counts from 0 to this value\n");
		return -1;
	}
	char *prio_argv[] = {argv[1], 0};
	return test_prio(1, prio_argv);
}

command test_prio_cmd = {"test_prio", test_prio_func, "Test priority scheduling"};
