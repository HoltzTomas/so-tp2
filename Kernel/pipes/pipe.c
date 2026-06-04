#include <pipe.h>
#include <scheduler.h>
#include <memoryManager.h>
#include <globals.h>
#include <lib.h>
#include <stddef.h>

static Pipe pipes[MAX_PIPES];

static int16_t fd_to_index(uint16_t id) {
    int16_t idx = (int16_t)id - BUILT_IN_DESCRIPTORS;
    if (idx < 0 || idx >= MAX_PIPES)
        return -1;
    return idx;
}

void pipe_manager_init(void) {
    memset(pipes, 0, sizeof(pipes));
}

int16_t pipe_create(void) {
    for (int i = 0; i < MAX_PIPES; i++) {
        if (!pipes[i].active) {
            Pipe *p = &pipes[i];
            memset(p->buffer, 0, PIPE_BUFFER_SIZE);
            p->read_pos = 0;
            p->write_pos = 0;
            p->count = 0;
            p->read_pid = -1;
            p->write_pid = -1;
            p->blocking = 0;
            p->eof = 0;
            p->active = 1;
            return (int16_t)(i + BUILT_IN_DESCRIPTORS);
        }
    }
    return -1;
}

int8_t pipe_open(uint16_t pid, uint16_t id, uint8_t mode) {
    int16_t idx = fd_to_index(id);
    if (idx < 0 || !pipes[idx].active)
        return -1;

    Pipe *p = &pipes[idx];

    if (mode == WRITE) {
        p->write_pid = (int16_t)pid;
    } else if (mode == READ) {
        p->read_pid = (int16_t)pid;
    } else {
        return -1;
    }

    return 0;
}

int8_t pipe_close(uint16_t pid, uint16_t id) {
    int16_t idx = fd_to_index(id);
    if (idx < 0 || !pipes[idx].active)
        return -1;

    Pipe *p = &pipes[idx];

    if ((int16_t)pid == p->write_pid) {
        p->write_pid = -1;
        p->eof = 1;
        if (p->blocking && p->read_pid != -1) {
            set_status(p->read_pid, READY);
            p->blocking = 0;
        }
    } else if ((int16_t)pid == p->read_pid) {
        p->read_pid = -1;
        if (p->blocking && p->write_pid != -1) {
            set_status(p->write_pid, READY);
            p->blocking = 0;
        }
    }

    if (p->write_pid == -1 && p->read_pid == -1) {
        p->active = 0;
    }

    return 0;
}

int64_t pipe_read(uint16_t id, char *buf, uint64_t len) {
    int16_t idx = fd_to_index(id);
    if (idx < 0 || !pipes[idx].active)
        return -1;

    Pipe *p = &pipes[idx];
    uint64_t bytes_read = 0;

    while (bytes_read < len) {
        while (p->count == 0 && !p->eof) {
            if (p->write_pid == -1) {
                p->eof = 1;
                break;
            }
            p->blocking = 1;
            set_status(get_pid(), BLOCKED);
            yield();
        }

        if (p->count == 0 && p->eof)
            break;

        while (p->count > 0 && bytes_read < len) {
            buf[bytes_read++] = p->buffer[p->read_pos];
            p->read_pos = (p->read_pos + 1) % PIPE_BUFFER_SIZE;
            p->count--;
        }

        if (p->blocking && p->write_pid != -1) {
            set_status(p->write_pid, READY);
            p->blocking = 0;
        }
    }

    return (int64_t)bytes_read;
}

int64_t pipe_write(uint16_t id, const char *buf, uint64_t len) {
    int16_t idx = fd_to_index(id);
    if (idx < 0 || !pipes[idx].active)
        return -1;

    Pipe *p = &pipes[idx];

    if (p->read_pid == -1)
        return -1;

    uint64_t bytes_written = 0;

    while (bytes_written < len) {
        while (p->count >= PIPE_BUFFER_SIZE) {
            if (p->read_pid == -1)
                return (int64_t)bytes_written;
            p->blocking = 1;
            set_status(get_pid(), BLOCKED);
            yield();
        }

        while (p->count < PIPE_BUFFER_SIZE && bytes_written < len) {
            p->buffer[p->write_pos] = buf[bytes_written++];
            p->write_pos = (p->write_pos + 1) % PIPE_BUFFER_SIZE;
            p->count++;
        }

        if (p->blocking && p->read_pid != -1) {
            set_status(p->read_pid, READY);
            p->blocking = 0;
        }
    }

    return (int64_t)bytes_written;
}
