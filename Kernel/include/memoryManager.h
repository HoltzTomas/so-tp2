#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>

#if !defined(FIRSTFIT) && !defined(BUDDY)
#define FIRSTFIT
#endif

void mm_init(uintptr_t start, uint32_t size);
void *mm_alloc(uint32_t size);
void mm_free(void *ptr);
void mm_get_stats(uint64_t *total, uint64_t *free);
const char *mm_get_name(void);

#endif
