#include "commands.h"
#include <stdio.h>

static int wc_func(int argc, char **argv) {
	int c;
	int line_count = 0;
	while ((c = getchar()) != (uint8_t)EOF_CHAR) {
		if (c == '\n')
			line_count++;
	}
	printf("Lines: %d\n", line_count);
	return 0;
}

command wc_cmd = {"wc", wc_func, "Count lines from stdin"};
