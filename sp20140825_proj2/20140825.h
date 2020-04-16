#ifndef __20140825_H__
#define __20140825_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "helpers.h"
#include "shell_commands.h"
#include "memory_commands.h"
#include "opcode_commands.h"
#include "assembler_commands.h"

#define MAX_INPUT_LEN 100
#define MAX_TOKEN_LEN 20
#define MAX_TOKENS 10

#define OPCODE_FILENAME "opcode.txt"

char input[MAX_INPUT_LEN];
char tokens[MAX_TOKENS][MAX_TOKEN_LEN];

int TOKEN_COUNT;

typedef enum _command {
    _none=0,
    _help,
    _dir,
    _quit, 
    _history, 
    _dump, 
    _edit, 
    _fill, 
    _reset, 
    _opcode, 
    _opcodelist, 
    _assemble,
    _type,
    _symbol,
}command;

void flush_input(void);
void flush_tokens(void);
int get_next_token_idx(char* str);
void tokenize_input(void);

command get_command(void);
error execute_instructions(command c);
void init(void);
void exit_program(void);

#endif
