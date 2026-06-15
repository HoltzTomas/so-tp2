#include "commands.h"
#include <stdio.h>

extern int test_sync(int argc, char *argv[]);

static int test_sync_func(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: test_sync <n> <use_sem>\n");
		printf("  n: number of increments/decrements\n");
		printf("  use_sem: 1 = with semaphores, 0 = without\n");
		return -1;
	}
	char *sync_argv[] = {argv[1], argv[2], 0};
	return test_sync(2, sync_argv);
}

command test_synchro_cmd = {"test_sync", test_sync_func, "Test sync (with/without semaphores)"};
