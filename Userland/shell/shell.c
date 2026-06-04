#include <stdio.h>
#include <stdlib.h>
#include "commands/commands.h"

#define MAX_BUFFER_SIZE 256
#define MAX_PIPE_COMMANDS 2
#define PROMPT "$ "

extern void throw_zero_division(void);
extern void throw_invalid_opcode(void);

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    int8_t hours;
    uint16_t year;
    uint8_t month;
    uint8_t day;
} Timestamp;

extern void sys_time(Timestamp *ts);

typedef struct {
    char *name;
    char *args[MAX_ARGS];
    int arg_count;
} Command;

typedef struct {
    Command commands[MAX_PIPE_COMMANDS];
    int command_count;
    int is_background;
} ParsedInput;

/* ---- built-in commands that don't spawn processes ---- */

static int time_func(int argc, char **argv) {
    Timestamp ts = {0};
    sys_time(&ts);
    printf("Date: %d/%d/%d  Time: %d:%d:%d\n",
           ts.day, ts.month, ts.year,
           ts.hours, ts.minutes, ts.seconds);
    return 0;
}
static command time_cmd = {"time", time_func, "Show current time"};

static int divzero_func(int argc, char **argv) {
    throw_zero_division();
    return 0;
}
static command divzero_cmd = {"divzero", divzero_func, "Test zero division exception"};

static int invalidop_func(int argc, char **argv) {
    throw_invalid_opcode();
    return 0;
}
static command invalidop_cmd = {"invalidop", invalidop_func, "Test invalid opcode exception"};

/* ---- command table ---- */

command *all_commands[] = {
    &help_cmd,
    &clear_cmd,
    &time_cmd,
    &mem_cmd,
    &ps_cmd,
    &loop_cmd,
    &kill_cmd,
    &nice_cmd,
    &block_cmd,
    &cat_cmd,
    &wc_cmd,
    &filter_cmd,
    &mvar_cmd,
    &divzero_cmd,
    &invalidop_cmd,
    0
};

static command *find_command(const char *name) {
    for (int i = 0; all_commands[i] != 0; i++) {
        if (strcmp(name, all_commands[i]->name) == 0)
            return all_commands[i];
    }
    return 0;
}

/* ---- input parsing ---- */

static void parse_single_command(char *input, Command *cmd) {
    cmd->arg_count = 0;
    cmd->name = 0;
    while (*input == ' ' || *input == '\t') input++;
    if (*input == '\0') return;

    char *token = strtok(input, " \t");
    if (token == 0) return;
    cmd->name = token;
    cmd->args[cmd->arg_count++] = token;
    while ((token = strtok(0, " \t")) != 0 && cmd->arg_count < MAX_ARGS - 1) {
        cmd->args[cmd->arg_count++] = token;
    }
    cmd->args[cmd->arg_count] = 0;
}

static void parse_input(char *input, ParsedInput *parsed) {
    parsed->command_count = 0;
    parsed->is_background = 0;

    uint64_t len = strlen(input);
    if (len > 0 && input[len - 1] == '&') {
        parsed->is_background = 1;
        input[len - 1] = '\0';
        len--;
        while (len > 0 && (input[len - 1] == ' ' || input[len - 1] == '\t'))
            input[--len] = '\0';
    }

    char *pipe_pos = strchr(input, '|');
    if (pipe_pos != 0) {
        *pipe_pos = '\0';
        parse_single_command(input, &parsed->commands[0]);
        if (parsed->commands[0].name != 0)
            parsed->command_count = 1;

        char *cmd2 = pipe_pos + 1;
        while (*cmd2 == ' ' || *cmd2 == '\t') cmd2++;
        if (*cmd2 != '\0') {
            parse_single_command(cmd2, &parsed->commands[1]);
            if (parsed->commands[1].name != 0)
                parsed->command_count = 2;
        }
    } else {
        parse_single_command(input, &parsed->commands[0]);
        if (parsed->commands[0].name != 0)
            parsed->command_count = 1;
    }
}

/* ---- execution ---- */

static void execute_single(Command *cmd, int is_background) {
    command *cmd_ptr = find_command(cmd->name);
    if (cmd_ptr == 0) {
        printf("Command '%s' not found. Type 'help'.\n", cmd->name);
        return;
    }

    if (is_background) {
        int16_t fds[3] = {DEV_NULL, STDOUT, STDERR};
        int64_t pid = sys_create_process(cmd_ptr->func, cmd->args, cmd->name, 1, fds);
        if (pid < 0) {
            printf("Failed to create background process\n");
            return;
        }
        printf("[%d] Started in background\n", (int)pid);
    } else {
        int16_t fds[3] = {STDIN, STDOUT, STDERR};
        int64_t pid = sys_create_process(cmd_ptr->func, cmd->args, cmd->name, 1, fds);
        if (pid < 0) {
            cmd_ptr->func(cmd->arg_count, cmd->args);
            return;
        }
        sys_waitpid((uint16_t)pid);
    }
}

static void execute_piped(ParsedInput *parsed) {
    command *cmd1 = find_command(parsed->commands[0].name);
    command *cmd2 = find_command(parsed->commands[1].name);

    if (cmd1 == 0) {
        printf("Command '%s' not found.\n", parsed->commands[0].name);
        return;
    }
    if (cmd2 == 0) {
        printf("Command '%s' not found.\n", parsed->commands[1].name);
        return;
    }

    int16_t pipe_id = sys_pipe_create();
    if (pipe_id < 0) {
        printf("Failed to create pipe\n");
        return;
    }

    int16_t fds1[3] = {
        parsed->is_background ? DEV_NULL : STDIN,
        pipe_id,
        STDERR
    };
    int64_t pid1 = sys_create_process(cmd1->func, parsed->commands[0].args,
                                      parsed->commands[0].name, 1, fds1);
    if (pid1 < 0) {
        printf("Failed to create first process\n");
        return;
    }

    int16_t fds2[3] = {pipe_id, STDOUT, STDERR};
    int64_t pid2 = sys_create_process(cmd2->func, parsed->commands[1].args,
                                      parsed->commands[1].name, 1, fds2);
    if (pid2 < 0) {
        printf("Failed to create second process\n");
        sys_kill((uint16_t)pid1, -1);
        return;
    }

    if (parsed->is_background) {
        printf("[%d,%d] Pipeline started in background\n", (int)pid1, (int)pid2);
    } else {
        sys_waitpid((uint16_t)pid1);
        sys_waitpid((uint16_t)pid2);
    }
}

/* ---- shell loop ---- */

int start(int argc, char **argv) {
    char buffer[MAX_BUFFER_SIZE];
    ParsedInput parsed;

    printf("SO TP2 - Kernel Shell\n");
    printf("Type 'help' for available commands.\n\n");

    while (1) {
        printf(PROMPT);
        uint64_t len = readline(buffer, MAX_BUFFER_SIZE);
        if (len == 0) continue;

        parse_input(buffer, &parsed);

        if (parsed.command_count == 2) {
            execute_piped(&parsed);
        } else if (parsed.command_count == 1) {
            execute_single(&parsed.commands[0], parsed.is_background);
        }
    }
    return 0;
}
