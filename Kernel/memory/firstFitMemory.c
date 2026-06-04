#ifdef FIRSTFIT

#include <stdint.h>
#include <stddef.h>
#include <lib.h>
#include <memoryManager.h>

typedef union MemBlock MemBlock;

union MemBlock {
    struct {
        MemBlock *next;
        uint32_t blockSize; // size in units (1 unit = sizeof(MemBlock))
    } metadata;
    uint64_t align;
};

static MemBlock sentinel;
static MemBlock *freeList = NULL;
static uint32_t totalMemory = 0;
static uint32_t freeMemory = 0;

#define BLOCK_SIZE sizeof(MemBlock)
#define MIN_BLOCK_UNITS 2

static inline uint32_t bytesToUnits(uint32_t bytes) {
    return (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

static inline uint32_t unitsToBytes(uint32_t units) {
    return units * BLOCK_SIZE;
}

void mm_init(uintptr_t start, uint32_t size) {
    uint32_t totalUnits = size / BLOCK_SIZE;
    if (totalUnits < MIN_BLOCK_UNITS)
        return;

    MemBlock *initialBlock = (MemBlock *)start;
    initialBlock->metadata.blockSize = totalUnits;

    totalMemory = unitsToBytes(totalUnits);
    freeMemory = totalMemory;

    sentinel.metadata.next = initialBlock;
    sentinel.metadata.blockSize = 0;

    initialBlock->metadata.next = &sentinel;

    freeList = &sentinel;
}

void *mm_alloc(uint32_t size) {
    if (size == 0)
        return NULL;

    uint32_t unitsNeeded = bytesToUnits(size) + 1; // +1 for header
    if (unitsNeeded < MIN_BLOCK_UNITS)
        unitsNeeded = MIN_BLOCK_UNITS;

    MemBlock *current, *previous;
    previous = freeList;

    for (current = previous->metadata.next; ;
         previous = current, current = current->metadata.next) {

        if (current->metadata.blockSize >= unitsNeeded) {
            MemBlock *allocatedBlock;

            if (current->metadata.blockSize == unitsNeeded) {
                previous->metadata.next = current->metadata.next;
                allocatedBlock = current;
            } else {
                current->metadata.blockSize -= unitsNeeded;
                allocatedBlock = current + current->metadata.blockSize;
                allocatedBlock->metadata.blockSize = unitsNeeded;
            }

            freeList = previous;
            freeMemory -= unitsToBytes(unitsNeeded);

            return (void *)(allocatedBlock + 1);
        }

        if (current == freeList)
            return NULL;
    }
}

void mm_free(void *ptr) {
    if (ptr == NULL)
        return;

    MemBlock *blockToFree = ((MemBlock *)ptr) - 1;
    freeMemory += unitsToBytes(blockToFree->metadata.blockSize);

    MemBlock *current;
    for (current = freeList;
         !(blockToFree > current && blockToFree < current->metadata.next);
         current = current->metadata.next) {
        if (current >= current->metadata.next &&
            (blockToFree > current || blockToFree < current->metadata.next))
            break;
    }

    // Coalesce with next block
    if (blockToFree + blockToFree->metadata.blockSize == current->metadata.next) {
        blockToFree->metadata.blockSize += current->metadata.next->metadata.blockSize;
        blockToFree->metadata.next = current->metadata.next->metadata.next;
    } else {
        blockToFree->metadata.next = current->metadata.next;
    }

    // Coalesce with previous block
    if (current + current->metadata.blockSize == blockToFree) {
        current->metadata.blockSize += blockToFree->metadata.blockSize;
        current->metadata.next = blockToFree->metadata.next;
    } else {
        current->metadata.next = blockToFree;
    }

    freeList = current;
}

void mm_get_stats(uint64_t *total, uint64_t *free) {
    if (total) *total = (uint64_t)totalMemory;
    if (free)  *free  = (uint64_t)freeMemory;
}

const char *mm_get_name(void) {
    return "First-Fit";
}

#endif
