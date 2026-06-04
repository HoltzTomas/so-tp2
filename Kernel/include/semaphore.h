#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <list.h>

#define MAX_SEMAPHORES 128

typedef struct {
    uint32_t value;
    int lock;
    List waiting_queue;
    List lock_queue;
    uint8_t active;
} Semaphore;

void semaphore_manager_init(void);
int8_t sem_init(uint16_t id, uint32_t initial_value);
int8_t sem_open(uint16_t id);
int8_t sem_close(uint16_t id);
int8_t sem_destroy(uint16_t id);
int8_t sem_wait(uint16_t id);
int8_t sem_post(uint16_t id);

#endif
