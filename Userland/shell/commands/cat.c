#include "commands.h"
#include <stdio.h>

static int cat_func(int argc, char **argv) {
	if (argc > 1) {
		for (int i = 1; i < argc; i++) {
			printf("%s\n", argv[i]);
		}
		return 0;
	}
	int c;
	while ((c = getchar()) != (uint8_t)EOF_CHAR) {
		putchar((char)c);
	}
	return 0;
}

command cat_cmd = {"cat", cat_func, "Print stdin as received"};
