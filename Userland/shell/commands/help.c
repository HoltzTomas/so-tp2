#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

static void print_padded(const char *str, int width) {
	int len = (int)strlen(str);
	printf("%s", str);
	for (int i = len; i < width; i++)
		putchar(' ');
}

static int is_test_command(const char *name) {
	return strncmp(name, "test_", 5) == 0;
}

static int help_func(int argc, char **argv) {
	printf("Available commands:\n");
	for (int i = 0; all_commands[i] != 0; i++) {
		if (!is_test_command(all_commands[i]->name)) {
			printf("  ");
			print_padded(all_commands[i]->name, 14);
			printf("- %s\n", all_commands[i]->description);
		}
	}

	printf("\nTests (provided by course staff):\n");
	for (int i = 0; all_commands[i] != 0; i++) {
		if (is_test_command(all_commands[i]->name)) {
			printf("  ");
			print_padded(all_commands[i]->name, 14);
			printf("- %s\n", all_commands[i]->description);
		}
	}

	return 0;
}

command help_cmd = {"help", help_func, "Show available commands"};
