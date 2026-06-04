#include "commands.h"
#include <stdio.h>

extern int test_processes(int argc, char *argv[]);

static int test_processes_func(int argc, char **argv) {
	if (argc <= 1) {
		printf("Usage: test-processes <max_processes>\n");
		return -1;
	}
	return test_processes(argc - 1, &argv[1]);
}

command test_processes_cmd = {"test-processes", test_processes_func, "Run process management test"};
