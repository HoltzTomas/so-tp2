#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_util.h>

#define MINOR_WAIT 1000000
#define WAIT 10000000

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

	for (int i = 0; i < 4; i++) {
		prio[i] = sys_create_process(
			(MainFunction)endless_loop_print, argvAux, "prio_test", 0, default_fds);
		if (prio[i] < 0) {
			printf("test_prio: ERROR creating process\n");
			return -1;
		}
	}

	bussy_wait(WAIT);
	printf("\nChanging priorities...\n");

	for (int i = 0; i < 4; i++) {
		switch (i % 4) {
		case 0: sys_nice((uint16_t)prio[i], 0); break;
		case 1: sys_nice((uint16_t)prio[i], 1); break;
		case 2: sys_nice((uint16_t)prio[i], 2); break;
		case 3: sys_nice((uint16_t)prio[i], 3); break;
		}
	}

	bussy_wait(WAIT);
	printf("\nKilling test processes...\n");

	for (int i = 0; i < 4; i++) {
		sys_kill((uint16_t)prio[i], -1);
		sys_waitpid((uint16_t)prio[i]);
	}

	printf("test_prio: done\n");
	return 0;
}
