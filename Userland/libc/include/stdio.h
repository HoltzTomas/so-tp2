#ifndef STDIO_H
#define STDIO_H

#include <stdint.h>
#include <stdarg.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define EOF (-1)

// Syscall declarations (defined in asm)
extern uint64_t sys_read(uint64_t fd, char *buf, uint64_t count);
extern uint64_t sys_write(uint64_t fd, const char *buf, uint64_t count);
extern void sys_clear(uint32_t color);
extern uint64_t sys_ticks(void);

uint64_t printf(const char *fmt, ...);
uint64_t putchar(char c);
void puts(const char *str);
uint8_t getchar(void);
uint64_t readline(char *buf, uint64_t max_len);

#endif
