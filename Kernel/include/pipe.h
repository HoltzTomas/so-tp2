#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>

#define PIPE_BUFFER_SIZE 1024
#define MAX_PIPES 16

typedef struct {
    char buffer[PIPE_BUFFER_SIZE];
    uint16_t read_pos;
    uint16_t write_pos;
    uint16_t count;
    int16_t read_pid;
    int16_t write_pid;
    uint8_t blocking;
    uint8_t eof;
    uint8_t active;
} Pipe;

void pipe_manager_init(void);
int16_t pipe_create(void);
int8_t pipe_open(uint16_t pid, uint16_t id, uint8_t mode);
int8_t pipe_close(uint16_t pid, uint16_t id);
int64_t pipe_read(uint16_t id, char *buf, uint64_t len);
int64_t pipe_write(uint16_t id, const char *buf, uint64_t len);

#endif
