#include "commands.h"
#include <stdio.h>

extern int test_mm(int argc, char *argv[]);

static int test_mm_func(int argc, char **argv) {
	if (argc <= 1) {
		printf("Usage: test_mm <max_memory>\n");
		return -1;
	}
	return test_mm(argc - 1, &argv[1]);
}

command test_mm_cmd = {"test_mm", test_mm_func, "Test memory manager"};
