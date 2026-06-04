#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdint.h>

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6

void exceptionDispatcher(int exception, uint64_t *rsp);
void set_restore_point(uint64_t rip, uint64_t rsp, uint64_t rbp);

#endif
