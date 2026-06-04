#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <registers.h>

// Syscall numbers
#define SYSCALL_READ    0
#define SYSCALL_WRITE   1
#define SYSCALL_TIME    2
#define SYSCALL_CLEAR   3
#define SYSCALL_TICKS   4
#define SYSCALL_MALLOC  5
#define SYSCALL_FREE    6
#define SYSCALL_MEMINFO 7

uint64_t intDispatcher(const registers_t *registers);

uint64_t sys_read(uint64_t fd, uint64_t buf, uint64_t count);
uint64_t sys_write(uint64_t fd, uint64_t buf, uint64_t count);

#endif
