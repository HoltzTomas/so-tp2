#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

static int kill_func(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: kill <pid>\n");
		return 1;
	}

	int pid = (int)atoi(argv[1]);
	if (pid <= 0) {
		printf("Invalid PID: %s\n", argv[1]);
		return 1;
	}

	int64_t result = sys_kill((uint16_t)pid, -1);
	if (result < 0) {
		printf("Failed to kill process %d\n", pid);
		return 1;
	}

	printf("Process %d killed\n", pid);
	return 0;
}

command kill_cmd = {"kill", kill_func, "Kill a process by PID"};
