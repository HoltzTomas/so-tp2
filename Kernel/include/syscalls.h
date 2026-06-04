#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>
#include <registers.h>

// Syscall numbers
#define SYSCALL_READ            0
#define SYSCALL_WRITE           1
#define SYSCALL_TIME            2
#define SYSCALL_CLEAR           3
#define SYSCALL_TICKS           4
#define SYSCALL_MALLOC          5
#define SYSCALL_FREE            6
#define SYSCALL_MEMINFO         7
#define SYSCALL_CREATE_PROCESS  8
#define SYSCALL_GETPID          9
#define SYSCALL_KILL            10
#define SYSCALL_BLOCK           11
#define SYSCALL_UNBLOCK         12
#define SYSCALL_YIELD           13
#define SYSCALL_WAITPID         14
#define SYSCALL_NICE            15
#define SYSCALL_PS              16
#define SYSCALL_SLEEP           17
#define SYSCALL_SEM_INIT        18
#define SYSCALL_SEM_OPEN        19
#define SYSCALL_SEM_CLOSE       20
#define SYSCALL_SEM_DESTROY     21
#define SYSCALL_SEM_WAIT        22
#define SYSCALL_SEM_POST        23
#define SYSCALL_PIPE_CREATE     24
#define SYSCALL_PIPE_OPEN       25
#define SYSCALL_PIPE_CLOSE      26

uint64_t intDispatcher(const registers_t *registers);

uint64_t sys_read(uint64_t fd, uint64_t buf, uint64_t count);
uint64_t sys_write(uint64_t fd, uint64_t buf, uint64_t count);

#endif
