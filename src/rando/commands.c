#include <stddef.h>
#include <libundermine.h>
#include "commands.h"

struct command rando_commands[RANDO_CMD_MAX] = {
    { "toggle menu",    COMMAND_PRESS,  NULL },
    { "return",         COMMAND_PRESS,  NULL }
};

uint16_t rando_binds[RANDO_CMD_MAX] = { 0 };