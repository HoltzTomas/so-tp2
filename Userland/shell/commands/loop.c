#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

static int loop_func(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: loop <seconds>\n");
		return -1;
	}

	int seconds = (int)atoi(argv[1]);
	if (seconds <= 0) {
		printf("Invalid seconds value\n");
		return -1;
	}

	int64_t pid = (int64_t)sys_getpid();
	int count = 0;

	while (1) {
		printf("[PID %d] Hello from loop! (iteration %d)\n", (int)pid, count);
		count++;
		sleep(seconds);
	}

	return 0;
}

command loop_cmd = {"loop", loop_func, "Print PID every N seconds"};
