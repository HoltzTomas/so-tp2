#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

uint64_t atoi(const char *str);
void itoa(uint64_t num, char *str);
uint64_t abs_val(int64_t x);
int strcmp(const char *str1, const char *str2);
uint64_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strncmp(const char *s1, const char *s2, uint64_t n);
void *memset(void *dest, int c, uint64_t n);
void *memcpy(void *dest, const void *src, uint64_t n);

// Memory management
extern void *sys_malloc(uint64_t size);
extern void sys_free(void *ptr);
extern void sys_mem_info(uint64_t *total, uint64_t *free);

// Process status enum (mirrors kernel)
typedef enum {
    READY = 0,
    RUNNING,
    BLOCKED,
    ZOMBIE
} ProcessStatus;

typedef struct {
    uint16_t pid;
    uint16_t parent_pid;
    char name[64];
    uint8_t priority;
    ProcessStatus status;
    void *stack_base;
    void *stack_pos;
    uint8_t is_foreground;
} ProcessInfo;

typedef int (*MainFunction)(int argc, char **args);

// Process management
extern int64_t sys_create_process(MainFunction code, char **args, char *name, uint8_t priority, int16_t *fds);
extern uint64_t sys_getpid(void);
extern int64_t sys_kill(uint16_t pid, int32_t retval);
extern int64_t sys_block(uint16_t pid);
extern int64_t sys_unblock(uint16_t pid);
extern void sys_yield(void);
extern int64_t sys_waitpid(uint16_t pid);
extern int64_t sys_nice(uint16_t pid, uint8_t new_priority);
extern int32_t sys_ps(ProcessInfo *info, uint32_t max_count);
extern void sys_sleep(uint64_t ticks);

// Semaphore management
extern int8_t sys_sem_init(uint16_t id, uint32_t initial_value);
extern int8_t sys_sem_open(uint16_t id);
extern int8_t sys_sem_close(uint16_t id);
extern int8_t sys_sem_destroy(uint16_t id);
extern int8_t sys_sem_wait(uint16_t id);
extern int8_t sys_sem_post(uint16_t id);

#endif
