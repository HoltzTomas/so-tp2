#include <syscalls.h>
#include <videoDriver.h>
#include <keyboardDriver.h>
#include <time.h>
#include <memoryManager.h>

static uint64_t (*syscall_table[])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = {
    [SYSCALL_READ]  = (uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)) sys_read,
    [SYSCALL_WRITE] = (uint64_t (*)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t)) sys_write,
};

#define SYSCALL_COUNT (sizeof(syscall_table) / sizeof(syscall_table[0]))

uint64_t intDispatcher(const registers_t *registers) {
    uint64_t syscall_num = registers->rax;

    if (syscall_num < SYSCALL_COUNT && syscall_table[syscall_num] != 0) {
        return syscall_table[syscall_num](
            registers->rdi,
            registers->rsi,
            registers->rdx,
            registers->r10,
            registers->r8,
            registers->r9
        );
    }

    // Handle syscalls not in the table via switch
    switch (syscall_num) {
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
        default:
            return (uint64_t)-1;
    }
}

uint64_t sys_read(uint64_t fd, uint64_t buf, uint64_t count) {
    // For now, only STDIN (fd 0) is supported via keyboard
    if (fd != 0)
        return (uint64_t)-1;

    char *buffer = (char *)buf;
    unsigned char c;
    uint64_t i;
    for (i = 0; i < count && (c = readNext()) != 0; i++) {
        buffer[i] = c;
    }
    return i;
}

uint64_t sys_write(uint64_t fd, uint64_t buf, uint64_t count) {
    // For now, fd 1 (stdout) and fd 2 (stderr) write to screen
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
