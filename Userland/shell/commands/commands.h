#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdint.h>

#define MAX_ARGS 10
#define DEV_NULL (-1)

typedef struct {
    char *name;
    int (*func)(int argc, char **argv);
    char *description;
} command;

extern command help_cmd;
extern command clear_cmd;
extern command mem_cmd;
extern command ps_cmd;
extern command loop_cmd;
extern command kill_cmd;
extern command nice_cmd;
extern command block_cmd;
extern command cat_cmd;
extern command wc_cmd;
extern command filter_cmd;
extern command mvar_cmd;

extern command *all_commands[];

#endif
