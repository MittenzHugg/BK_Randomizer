/*
* commands.h
*
* definitions for kz commands
*/

#ifndef _COMMANDS_H
#define _COMMANDS_H

#include <stdint.h>

enum {
    RANDO_CMD_TOGGLE_MENU,
    RANDO_CMD_RETURN,
    RANDO_CMD_MAX
};

enum command_type {
    COMMAND_HOLD,
    COMMAND_PRESS
};

struct command {
    char               *text;
    enum command_type   type;
    void              (*proc)();
};

void    command_return          (void);

extern struct command rando_commands[RANDO_CMD_MAX];
extern uint16_t rando_binds[RANDO_CMD_MAX];
#endif