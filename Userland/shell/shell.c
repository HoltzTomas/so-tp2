#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 256
#define PROMPT "$ "
#define MAX_PS_ENTRIES 20

extern void throw_zero_division(void);
extern void throw_invalid_opcode(void);
extern void sys_clear(uint32_t color);

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    int8_t hours;
    uint16_t year;
    uint8_t month;
    uint8_t day;
} Timestamp;

extern void sys_time(Timestamp *ts);

static const char *status_names[] = {"READY", "RUNNING", "BLOCKED", "ZOMBIE"};

static void cmd_help(void) {
    printf("Available commands:\n");
    printf("  help        - Show this help\n");
    printf("  clear       - Clear screen\n");
    printf("  time        - Show current time\n");
    printf("  mem         - Show memory info\n");
    printf("  ps          - Show running processes\n");
    printf("  divzero     - Test zero division exception\n");
    printf("  invalidop   - Test invalid opcode exception\n");
}

static void cmd_time(void) {
    Timestamp ts = {0};
    sys_time(&ts);
    printf("Date: %d/%d/%d  Time: %d:%d:%d\n",
           ts.day, ts.month, ts.year,
           ts.hours, ts.minutes, ts.seconds);
}

static void cmd_mem(void) {
    uint64_t total = 0, free_mem = 0;
    sys_mem_info(&total, &free_mem);
    printf("Total: %d bytes\n", (int)total);
    printf("Used:  %d bytes\n", (int)(total - free_mem));
    printf("Free:  %d bytes\n", (int)free_mem);
}

static void cmd_ps(void) {
    ProcessInfo info[MAX_PS_ENTRIES];
    int32_t count = sys_ps(info, MAX_PS_ENTRIES);
    if (count <= 0) {
        printf("No processes found.\n");
        return;
    }

    printf("PID  PPID  PRIO  STATUS   FG  NAME\n");
    printf("---  ----  ----  ------   --  ----\n");
    for (int i = 0; i < count; i++) {
        const char *st = (info[i].status < 4) ? status_names[info[i].status] : "???";
        printf("%d    %d     %d     %s  %s  %s\n",
               info[i].pid, info[i].parent_pid, info[i].priority,
               st,
               info[i].is_foreground ? "Y" : "N",
               info[i].name);
    }
}

static void process_command(char *input) {
    while (*input == ' ' || *input == '\t') input++;
    if (*input == '\0') return;

    if (strcmp(input, "help") == 0) {
        cmd_help();
    } else if (strcmp(input, "clear") == 0) {
        sys_clear(0);
    } else if (strcmp(input, "time") == 0) {
        cmd_time();
    } else if (strcmp(input, "mem") == 0) {
        cmd_mem();
    } else if (strcmp(input, "ps") == 0) {
        cmd_ps();
    } else if (strcmp(input, "divzero") == 0) {
        throw_zero_division();
    } else if (strcmp(input, "invalidop") == 0) {
        throw_invalid_opcode();
    } else {
        printf("Unknown command: %s\nType 'help' for available commands.\n", input);
    }
}

int start(int argc, char **argv) {
    char buffer[MAX_BUFFER_SIZE];

    printf("SO TP2 - Kernel Shell\n");
    printf("Type 'help' for available commands.\n\n");

    while (1) {
        printf(PROMPT);
        uint64_t len = readline(buffer, MAX_BUFFER_SIZE);
        if (len > 0) {
            process_command(buffer);
        }
    }
    return 0;
}
