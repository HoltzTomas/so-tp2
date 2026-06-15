#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_WRITERS 26
#define MVAR_MUTEX 100
#define MVAR_READ_SEM 101
#define MVAR_WRITE_SEM 102

static char shared_mvar = 0;

static void build_name(char *dest, const char *prefix, int num) {
	char num_str[12];
	strcpy(dest, prefix);
	itoa(num, num_str);
	strcat(dest, num_str);
}

static void busy_wait(uint64_t n) {
	for (uint64_t i = 0; i < n; i++);
}

static int writer_process(int argc, char **argv) {
	if (argc < 1) return -1;
	int writer_id = (int)atoi(argv[0]) % MAX_WRITERS;
	char my_letter = 'A' + writer_id;

	if (sys_sem_open(MVAR_MUTEX) < 0 ||
	    sys_sem_open(MVAR_READ_SEM) < 0 ||
	    sys_sem_open(MVAR_WRITE_SEM) < 0) {
		printf("Writer: ERROR opening semaphores\n");
		return -1;
	}

	while (1) {
		busy_wait(rand() % 100000);
		if (sys_sem_wait(MVAR_WRITE_SEM) < 0) return -1;
		if (sys_sem_wait(MVAR_MUTEX) < 0) return -1;
		shared_mvar = my_letter;
		if (sys_sem_post(MVAR_MUTEX) < 0) return -1;
		if (sys_sem_post(MVAR_READ_SEM) < 0) return -1;
	}
	return 0;
}

static int reader_process(int argc, char **argv) {
	if (argc < 1) return -1;

	if (sys_sem_open(MVAR_MUTEX) < 0 ||
	    sys_sem_open(MVAR_READ_SEM) < 0 ||
	    sys_sem_open(MVAR_WRITE_SEM) < 0) {
		printf("Reader: ERROR opening semaphores\n");
		return -1;
	}

	while (1) {
		busy_wait(rand() % 100000);
		if (sys_sem_wait(MVAR_READ_SEM) < 0) return -1;
		if (sys_sem_wait(MVAR_MUTEX) < 0) return -1;
		char value = shared_mvar;
		if (sys_sem_post(MVAR_MUTEX) < 0) return -1;
		if (sys_sem_post(MVAR_WRITE_SEM) < 0) return -1;
		putchar(value);
	}
	return 0;
}

static int mvar_func(int argc, char **argv) {
	if (argc != 3) {
		printf("Usage: mvar <num_writers> <num_readers>\n");
		return -1;
	}

	int num_writers = (int)atoi(argv[1]);
	int num_readers = (int)atoi(argv[2]);

	if (num_writers <= 0 || num_readers <= 0) {
		printf("Error: num_writers and num_readers must be positive\n");
		return -1;
	}
	if (num_writers > MAX_WRITERS) {
		printf("Error: max 26 writers (A-Z)\n");
		return -1;
	}

	if (sys_sem_init(MVAR_MUTEX, 1) < 0) {
		printf("mvar: ERROR creating mutex\n");
		return -1;
	}
	if (sys_sem_init(MVAR_READ_SEM, 0) < 0) {
		printf("mvar: ERROR creating read sem\n");
		sys_sem_destroy(MVAR_MUTEX);
		return -1;
	}
	if (sys_sem_init(MVAR_WRITE_SEM, 1) < 0) {
		printf("mvar: ERROR creating write sem\n");
		sys_sem_destroy(MVAR_MUTEX);
		sys_sem_destroy(MVAR_READ_SEM);
		return -1;
	}

	int16_t default_fds[3] = {STDIN, STDOUT, STDERR};

	for (int i = 0; i < num_writers; i++) {
		char id_str[12];
		itoa(i, id_str);
		char *writer_args[] = {id_str, 0};
		char writer_name[32];
		build_name(writer_name, "writer_", i);
		int64_t pid = sys_create_process((MainFunction)writer_process, writer_args, writer_name, 2, default_fds);
		if (pid < 0) {
			printf("mvar: ERROR creating writer\n");
			return -1;
		}
	}

	for (int i = 0; i < num_readers; i++) {
		char id_str[12];
		itoa(i, id_str);
		char *reader_args[] = {id_str, 0};
		char reader_name[32];
		build_name(reader_name, "reader_", i);
		int64_t pid = sys_create_process((MainFunction)reader_process, reader_args, reader_name, 2, default_fds);
		if (pid < 0) {
			printf("mvar: ERROR creating reader\n");
			return -1;
		}
	}

	return 0;
}

command mvar_cmd = {"mvar", mvar_func, "Multiple readers/writers with MVar"};
