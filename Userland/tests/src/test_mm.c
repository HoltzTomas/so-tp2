#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <test_util.h>

#define MAX_BLOCKS 128

typedef struct {
	void *address;
	uint32_t size;
} mm_rq;

int test_mm(int argc, char *argv[]) {
	mm_rq mm_rqs[MAX_BLOCKS];
	uint8_t rq;
	uint32_t total;
	uint64_t max_memory;

	if (argc != 1)
		return -1;

	if ((max_memory = satoi(argv[0])) <= 0)
		return -1;

	while (1) {
		rq = 0;
		total = 0;

		while (rq < MAX_BLOCKS && total < max_memory) {
			mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
			mm_rqs[rq].address = sys_malloc(mm_rqs[rq].size);

			if (mm_rqs[rq].address) {
				total += mm_rqs[rq].size;
				rq++;
			}
		}

		uint32_t i;
		for (i = 0; i < rq; i++)
			if (mm_rqs[i].address)
				memset(mm_rqs[i].address, i, mm_rqs[i].size);

		for (i = 0; i < rq; i++)
			if (mm_rqs[i].address)
				if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
					printf("test_mm ERROR\n");
					return -1;
				}

		for (i = 0; i < rq; i++)
			if (mm_rqs[i].address)
				sys_free(mm_rqs[i].address);
	}
}
