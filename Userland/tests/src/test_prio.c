#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_util.h>

#define MINOR_WAIT 100000
#define WAIT_TICKS 90

static int64_t prio[4];

static int endless_loop_print(int argc, char *argv[]) {
	int64_t pid = (int64_t)sys_getpid();
	while (1) {
		printf("%d ", (int)pid);
		bussy_wait(MINOR_WAIT);
	}
	return 0;
}

int test_prio(int argc, char *argv[]) {
	int16_t default_fds[3] = {STDIN, STDOUT, STDERR};
	char *argvAux[] = {0};

	printf("Creating 4 processes with same priority...\n");

	for (int i = 0; i < 4; i++) {
		prio[i] = sys_create_process(
			(MainFunction)endless_loop_print, argvAux, "prio_test", 0, default_fds);
		if (prio[i] < 0) {
			printf("test_prio: ERROR creating process\n");
			return -1;
		}
	}

	sys_sleep(WAIT_TICKS);
	printf("\nChanging priorities...\n");
	printf("PID %d -> prio 0, PID %d -> prio 1, PID %d -> prio 2, PID %d -> prio 3\n",
	       (int)prio[0], (int)prio[1], (int)prio[2], (int)prio[3]);

	for (int i = 0; i < 4; i++) {
		sys_nice((uint16_t)prio[i], (uint8_t)i);
	}

	sys_sleep(WAIT_TICKS);
	printf("\nKilling test processes...\n");

	for (int i = 0; i < 4; i++) {
		sys_kill((uint16_t)prio[i], -1);
		sys_waitpid((uint16_t)prio[i]);
	}

	printf("test_prio: done\n");
	return 0;
}
