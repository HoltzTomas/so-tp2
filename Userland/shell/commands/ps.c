#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_PS_ENTRIES 20

static const char *status_names[] = {"READY", "RUNNING", "BLOCKED", "ZOMBIE"};

static int ps_func(int argc, char **argv) {
	ProcessInfo info[MAX_PS_ENTRIES];
	int32_t count = sys_ps(info, MAX_PS_ENTRIES);
	if (count <= 0) {
		printf("No processes found.\n");
		return -1;
	}

	printf("PID  PPID  PRIO  STATUS   FG  NAME\n");
	printf("---  ----  ----  ------   --  ----\n");
	for (int i = 0; i < count; i++) {
		const char *st = (info[i].status < 4) ? status_names[info[i].status] : "???";
		printf("%d    %d     %d     %s  %s  %s\n",
		       info[i].pid, info[i].parent_pid, info[i].priority,
		       st,
		       info[i].is_foreground ? "Y" : "N",
		       info[i].name);
	}
	return 0;
}

command ps_cmd = {"ps", ps_func, "Show running processes"};
