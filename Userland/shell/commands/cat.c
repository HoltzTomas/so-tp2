#include "commands.h"
#include <stdio.h>

static int cat_func(int argc, char **argv) {
	int c;
	while ((c = getchar()) != (uint8_t)EOF_CHAR) {
		putchar((char)c);
	}
	return 0;
}

command cat_cmd = {"cat", cat_func, "Print stdin as received"};
