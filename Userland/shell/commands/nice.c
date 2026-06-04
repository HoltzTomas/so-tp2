#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

static int nice_func(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: nice <pid> <priority>\n");
		printf("Priority: 0 (lowest) to 4 (highest)\n");
		return 1;
	}

	int pid = (int)atoi(argv[1]);
	if (pid <= 0) {
		printf("Invalid PID: %s\n", argv[1]);
		return 1;
	}

	int priority = (int)atoi(argv[2]);
	if (priority < 0 || priority > 4) {
		printf("Invalid priority: %s (must be 0-4)\n", argv[2]);
		return 1;
	}

	int64_t result = sys_nice((uint16_t)pid, (uint8_t)priority);
	if (result < 0) {
		printf("Failed to change priority for process %d\n", pid);
		return 1;
	}

	printf("Process %d priority changed to %d\n", pid, priority);
	return 0;
}

command nice_cmd = {"nice", nice_func, "Change process priority"};
