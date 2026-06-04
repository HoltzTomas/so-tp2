#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_util.h>

enum TestState {
	TEST_RUNNING,
	TEST_BLOCKED,
	TEST_KILLED
};

typedef struct {
	int32_t pid;
	enum TestState state;
} p_rq;

static int endless_loop_func(int argc, char **argv) {
	while (1) {
		sys_yield();
	}
	return 0;
}

int test_processes(int argc, char *argv[]) {
	uint8_t rq;
	uint8_t alive = 0;
	uint8_t action;
	uint64_t max_processes;
	char *argvAux[] = {0};

	if (argc != 1)
		return -1;

	if ((max_processes = satoi(argv[0])) <= 0)
		return -1;

	p_rq p_rqs[max_processes];
	int16_t default_fds[3] = {STDIN, STDOUT, STDERR};

	while (1) {
		for (rq = 0; rq < max_processes; rq++) {
			p_rqs[rq].pid = (int32_t)sys_create_process(
				(MainFunction)endless_loop_func, argvAux, "endless_loop", 0, default_fds);

			if (p_rqs[rq].pid == -1) {
				printf("test_processes: ERROR creating process\n");
				return -1;
			} else {
				p_rqs[rq].state = TEST_RUNNING;
				alive++;
			}
		}

		while (alive > 0) {
			for (rq = 0; rq < max_processes; rq++) {
				action = GetUniform(100) % 2;

				switch (action) {
				case 0:
					if (p_rqs[rq].state == TEST_RUNNING || p_rqs[rq].state == TEST_BLOCKED) {
						if (sys_kill((uint16_t)p_rqs[rq].pid, -1) == -1) {
							printf("test_processes: ERROR killing process\n");
							return -1;
						}
						p_rqs[rq].state = TEST_KILLED;
						alive--;
						sys_waitpid((uint16_t)p_rqs[rq].pid);
					}
					break;

				case 1:
					if (p_rqs[rq].state == TEST_RUNNING) {
						if (sys_block((uint16_t)p_rqs[rq].pid) == -1) {
							printf("test_processes: ERROR blocking process\n");
							return -1;
						}
						p_rqs[rq].state = TEST_BLOCKED;
					}
					break;
				}
			}

			for (rq = 0; rq < max_processes; rq++)
				if (p_rqs[rq].state == TEST_BLOCKED && GetUniform(100) % 2) {
					if (sys_unblock((uint16_t)p_rqs[rq].pid) == -1) {
						printf("test_processes: ERROR unblocking process\n");
						return -1;
					}
					p_rqs[rq].state = TEST_RUNNING;
				}
		}
	}
}
