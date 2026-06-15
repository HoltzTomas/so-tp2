#include "commands.h"
#include <stdio.h>

extern int test_processes(int argc, char *argv[]);

static int test_processes_func(int argc, char **argv) {
	if (argc <= 1) {
		printf("Usage: test_proc <max_processes>\n");
		return -1;
	}
	return test_processes(argc - 1, &argv[1]);
}

command test_processes_cmd = {"test_proc", test_processes_func, "Test process management"};
