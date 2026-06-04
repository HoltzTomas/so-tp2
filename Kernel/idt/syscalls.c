#include <syscalls.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <time.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <globals.h>
#include <semaphore.h>
#include <pipe.h>
#include <stddef.h>

uint64_t intDispatcher(const registers_t *registers) {
    uint64_t syscall_num = registers->rax;

    switch (syscall_num) {
        case SYSCALL_READ:
            return sys_read(registers->rdi, registers->rsi, registers->rdx);
        case SYSCALL_WRITE:
            return sys_write(registers->rdi, registers->rsi, registers->rdx);
        case SYSCALL_TIME:
            _sti();
            getTime((Timestamp *)registers->rdi);
            _cli();
            return 0;
        case SYSCALL_CLEAR:
            clear_screen((uint32_t)registers->rdi);
            return 0;
        case SYSCALL_TICKS:
            return (uint64_t)ticks_elapsed();
        case SYSCALL_MALLOC:
            return (uint64_t)mm_alloc((uint32_t)registers->rdi);
        case SYSCALL_FREE:
            mm_free((void *)registers->rdi);
            return 0;
        case SYSCALL_MEMINFO: {
            uint64_t *total = (uint64_t *)registers->rdi;
            uint64_t *free_mem = (uint64_t *)registers->rsi;
            mm_get_stats(total, free_mem);
            return 0;
        }
        case SYSCALL_CREATE_PROCESS: {
            MainFunction code = (MainFunction)registers->rdi;
            char **args = (char **)registers->rsi;
            char *name = (char *)registers->rdx;
            uint8_t priority = (uint8_t)registers->r10;
            int16_t *fds = (int16_t *)registers->r8;
            return (uint64_t)(int64_t)create_process(code, args, name, priority, fds, 0);
        }
        case SYSCALL_GETPID:
            return (uint64_t)get_pid();
        case SYSCALL_KILL: {
            int32_t result = kill_process((uint16_t)registers->rdi, (int32_t)registers->rsi);
            return (uint64_t)(int64_t)result;
        }
        case SYSCALL_BLOCK: {
            int8_t result = set_status((uint16_t)registers->rdi, BLOCKED);
            return (uint64_t)(int64_t)result;
        }
        case SYSCALL_UNBLOCK: {
            int8_t result = set_status((uint16_t)registers->rdi, READY);
            return (uint64_t)(int64_t)result;
        }
        case SYSCALL_YIELD:
            yield();
            return 0;
        case SYSCALL_WAITPID: {
            int32_t result = waitpid((uint16_t)registers->rdi);
            return (uint64_t)(int64_t)result;
        }
        case SYSCALL_NICE: {
            if (registers->rsi >= NUM_PRIORITIES)
                return (uint64_t)(int64_t)-1;
            int8_t result = set_priority((uint16_t)registers->rdi, (uint8_t)registers->rsi);
            return (uint64_t)(int64_t)result;
        }
        case SYSCALL_PS: {
            ProcessInfo *info = (ProcessInfo *)registers->rdi;
            uint32_t max_count = (uint32_t)registers->rsi;
            int32_t result = get_process_info(info, max_count);
            return (uint64_t)(int64_t)result;
        }
        case SYSCALL_SLEEP: {
            uint64_t start = ticks_elapsed();
            uint64_t target = (uint64_t)registers->rdi;
            while (ticks_elapsed() - start < target)
                _hlt();
            return 0;
        }
        case SYSCALL_SEM_INIT:
            return (uint64_t)(int64_t)sem_init((uint16_t)registers->rdi, (uint32_t)registers->rsi);
        case SYSCALL_SEM_OPEN:
            return (uint64_t)(int64_t)sem_open((uint16_t)registers->rdi);
        case SYSCALL_SEM_CLOSE:
            return (uint64_t)(int64_t)sem_close((uint16_t)registers->rdi);
        case SYSCALL_SEM_DESTROY:
            return (uint64_t)(int64_t)sem_destroy((uint16_t)registers->rdi);
        case SYSCALL_SEM_WAIT:
            return (uint64_t)(int64_t)sem_wait((uint16_t)registers->rdi);
        case SYSCALL_SEM_POST:
            return (uint64_t)(int64_t)sem_post((uint16_t)registers->rdi);
        case SYSCALL_PIPE_CREATE:
            return (uint64_t)(int64_t)pipe_create();
        case SYSCALL_PIPE_OPEN:
            return (uint64_t)(int64_t)pipe_open((uint16_t)registers->rdi, (uint16_t)registers->rsi, (uint8_t)registers->rdx);
        case SYSCALL_PIPE_CLOSE:
            return (uint64_t)(int64_t)pipe_close((uint16_t)registers->rdi, (uint16_t)registers->rsi);
        default:
            return (uint64_t)-1;
    }
}

uint64_t sys_read(uint64_t fd, uint64_t buf, uint64_t count) {
    Process *current = get_current_process();

    if (current != NULL && fd == STDIN) {
        int16_t actual_fd = current->file_descriptors[0];
        if (actual_fd >= BUILT_IN_DESCRIPTORS)
            return (uint64_t)pipe_read((uint16_t)actual_fd, (char *)buf, count);
        if (actual_fd == DEV_NULL)
            return 0;
        fd = (uint64_t)actual_fd;
    }

    if (fd != 0)
        return (uint64_t)-1;

    char *buffer = (char *)buf;
    uint64_t i;
    for (i = 0; i < count; i++) {
        char c = readNextBlocking();
        if (c == EOF_CHAR)
            break;
        buffer[i] = c;
        if (c == '\n')
            return i + 1;
    }
    return i;
}

uint64_t sys_write(uint64_t fd, uint64_t buf, uint64_t count) {
    Process *current = get_current_process();

    if (current != NULL && fd <= 2) {
        int16_t actual_fd = current->file_descriptors[fd == 2 ? 2 : 1];
        if (actual_fd >= BUILT_IN_DESCRIPTORS)
            return (uint64_t)pipe_write((uint16_t)actual_fd, (const char *)buf, count);
        if (actual_fd == DEV_NULL)
            return count;
        fd = (uint64_t)actual_fd;
    }

    if (fd != 1 && fd != 2)
        return (uint64_t)-1;

    const char *buffer = (const char *)buf;
    uint32_t color = get_text_color();
    uint64_t i = 0;
    for (; i < count && buffer[i] != '\0'; i++) {
        draw_char(buffer[i], color);
    }
    return i;
}
