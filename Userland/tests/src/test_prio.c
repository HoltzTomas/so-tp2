#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_util.h>

#define TOTAL_PROCESSES 3
#define LOWEST 0
#define MEDIUM 2
#define HIGHEST 4

static int64_t prio[TOTAL_PROCESSES] = {LOWEST, MEDIUM, HIGHEST};
static uint64_t max_value = 0;

static int zero_to_max(int argc, char *argv[]) {
	uint64_t value = 0;
	while (value++ != max_value)
		;
	printf("PROCESS %d DONE!\n", (int)sys_getpid());
	return 0;
}

int test_prio(int argc, char *argv[]) {
	int64_t pids[TOTAL_PROCESSES];
	char *ztm_argv[] = {0};
	int16_t default_fds[3] = {STDIN, STDOUT, STDERR};

	if (argc != 1)
		return -1;

	if ((max_value = (uint64_t)satoi(argv[0])) <= 0)
		return -1;

	printf("SAME PRIORITY...\n");
	for (int i = 0; i < TOTAL_PROCESSES; i++) {
		pids[i] = sys_create_process(
			(MainFunction)zero_to_max, ztm_argv, "zero_to_max", 0, default_fds);
		if (pids[i] < 0) {
			printf("test_prio: ERROR creating process\n");
			return -1;
		}
	}

	for (int i = 0; i < TOTAL_PROCESSES; i++)
		sys_waitpid((uint16_t)pids[i]);

	printf("\nSAME PRIORITY, THEN CHANGE IT...\n");
	for (int i = 0; i < TOTAL_PROCESSES; i++) {
		pids[i] = sys_create_process(
			(MainFunction)zero_to_max, ztm_argv, "zero_to_max", 0, default_fds);
		if (pids[i] < 0) {
			printf("test_prio: ERROR creating process\n");
			return -1;
		}
		sys_nice((uint16_t)pids[i], (uint8_t)prio[i]);
		printf("  PROCESS %d NEW PRIORITY: %d\n", (int)pids[i], (int)prio[i]);
	}

	for (int i = 0; i < TOTAL_PROCESSES; i++)
		sys_waitpid((uint16_t)pids[i]);

	printf("\nSAME PRIORITY, THEN CHANGE IT WHILE BLOCKED...\n");
	for (int i = 0; i < TOTAL_PROCESSES; i++) {
		pids[i] = sys_create_process(
			(MainFunction)zero_to_max, ztm_argv, "zero_to_max", 0, default_fds);
		if (pids[i] < 0) {
			printf("test_prio: ERROR creating process\n");
			return -1;
		}
		sys_block((uint16_t)pids[i]);
		sys_nice((uint16_t)pids[i], (uint8_t)prio[i]);
		printf("  PROCESS %d NEW PRIORITY: %d\n", (int)pids[i], (int)prio[i]);
	}

	for (int i = 0; i < TOTAL_PROCESSES; i++)
		sys_unblock((uint16_t)pids[i]);

	for (int i = 0; i < TOTAL_PROCESSES; i++)
		sys_waitpid((uint16_t)pids[i]);

	printf("test_prio: done\n");
	return 0;
}
