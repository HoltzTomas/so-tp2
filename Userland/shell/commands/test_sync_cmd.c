#include "commands.h"
#include <stdio.h>

extern int test_sync(int argc, char *argv[]);

static int test_synchro_func(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: test-synchro <n>\n");
		return -1;
	}
	char *sync_argv[] = {argv[1], "1"};
	return test_sync(2, sync_argv);
}

command test_synchro_cmd = {"test-synchro", test_synchro_func, "Run sync test WITH semaphores"};

static int test_no_synchro_func(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: test-no-synchro <n>\n");
		return -1;
	}
	char *sync_argv[] = {argv[1], "0"};
	return test_sync(2, sync_argv);
}

command test_no_synchro_cmd = {"test-no-synchro", test_no_synchro_func, "Run sync test WITHOUT semaphores"};
