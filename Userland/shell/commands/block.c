#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_PROCESSES 20

static int block_func(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: block <pid>\n");
		return 1;
	}

	int pid = (int)atoi(argv[1]);
	if (pid <= 0) {
		printf("Invalid PID: %s\n", argv[1]);
		return 1;
	}

	ProcessInfo info[MAX_PROCESSES];
	int count = sys_ps(info, MAX_PROCESSES);
	if (count < 0) {
		printf("Failed to get process info\n");
		return 1;
	}

	ProcessStatus current_status = READY;
	int found = 0;
	for (int i = 0; i < count; i++) {
		if (info[i].pid == pid) {
			current_status = info[i].status;
			found = 1;
			break;
		}
	}

	if (!found) {
		printf("Process %d not found\n", pid);
		return 1;
	}

	if (current_status == ZOMBIE) {
		printf("Cannot block/unblock zombie process %d\n", pid);
		return 1;
	}

	int64_t result;
	const char *action;

	if (current_status == BLOCKED) {
		result = sys_unblock((uint16_t)pid);
		action = "unblocked";
	} else {
		result = sys_block((uint16_t)pid);
		action = "blocked";
	}

	if (result < 0) {
		printf("Failed to toggle block state for process %d\n", pid);
		return 1;
	}

	printf("Process %d %s\n", pid, action);
	return 0;
}

command block_cmd = {"block", block_func, "Toggle process block state"};
