#include "commands.h"
#include <stdio.h>
#include <stdlib.h>

static int filter_func(int argc, char **argv) {
	int c;
	while ((c = getchar()) != (uint8_t)EOF_CHAR) {
		char lower = tolower((char)c);
		if (lower == 'a' || lower == 'e' || lower == 'i' ||
		    lower == 'o' || lower == 'u') {
			putchar((char)c);
		}
	}
	putchar('\n');
	return 0;
}

command filter_cmd = {"filter", filter_func, "Filter vowels from stdin"};
