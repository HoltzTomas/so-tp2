#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_PS_ENTRIES 20

static const char *status_names[] = {"READY", "RUNNING", "BLOCKED", "ZOMBIE"};

static void print_hex(uint64_t val) {
	char hex[17];
	int i = 15;
	hex[16] = '\0';
	for (; i >= 0; i--) {
		int digit = val & 0xF;
		hex[i] = digit < 10 ? '0' + digit : 'A' + digit - 10;
		val >>= 4;
	}
	char *p = hex;
	while (*p == '0' && *(p + 1) != '\0') p++;
	printf("0x%s", p);
}

static int ps_func(int argc, char **argv) {
	ProcessInfo info[MAX_PS_ENTRIES];
	int32_t count = sys_ps(info, MAX_PS_ENTRIES);
	if (count <= 0) {
		printf("No processes found.\n");
		return -1;
	}

	for (int i = 0; i < count; i++) {
		const char *st = (info[i].status < 4) ? status_names[info[i].status] : "???";
		printf("PID: %d  PPID: %d  Prio: %d  Status: %s  FG: %s  Name: %s\n",
		       info[i].pid, info[i].parent_pid, info[i].priority,
		       st,
		       info[i].is_foreground ? "Y" : "N",
		       info[i].name);
		printf("  RSP: ");
		print_hex((uint64_t)info[i].stack_pos);
		printf("  RBP: ");
		print_hex((uint64_t)info[i].stack_base);
		printf("\n");
	}
	return 0;
}

command ps_cmd = {"ps", ps_func, "Show running processes"};
