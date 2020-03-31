#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "helpers.h"
#include "shell_commands.h"

typedef enum _command {
    _help=0, _dir, _quit, _history, _dump, _edit, _fill, _reset, _opcode, _opcodelist, _none
} command;

command process_instructions(void);

