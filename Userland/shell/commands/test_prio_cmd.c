#include "commands.h"
#include <stdio.h>

extern int test_prio(int argc, char *argv[]);

static int test_prio_func(int argc, char **argv) {
	return test_prio(0, 0);
}

command test_prio_cmd = {"test_prio", test_prio_func, "Test priority scheduling"};
