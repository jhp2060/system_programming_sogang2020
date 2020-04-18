#ifndef __ASSEMBLE_COMMANDS_H__
#define __ASSEMBLE_COMMANDS_H__

#include "helpers.h"
#include "opcode_commands.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>

#define LINE_MULTIPLIER 5
#define MAX_LINE_LEN 128
#define MAX_LABEL_LEN 30
#define MAX_OPCODE_LEN 30
#define MAX_OPERAND_LEN 30

typedef enum _linetype {
    LT_START,
    LT_END,
    LT_COMMENT,
    LT_OPCODE,
    LT_BASE,
    LT_WORD,
    LT_RESW,
    LT_RESB,
    LT_BYTE,
    LT_NOT_A_LINE,
}linetype;

typedef struct _sym_node {
    char symbol[MAX_LABEL_LEN];
    int address;
    struct _sym_node* next;
}sym_node;

sym_node* SYMTAB;

error assemble(char* filename, int token_count);

// functions for assemble
error pass1(FILE* fp, int* program_length);

void read_line(FILE* fp, char* line);
linetype parse(char* line, char* label, char* opcode, char* op1, char* op2);
void init_symtab(void);
void free_symtab(void);
void push_symtab(char* symbol, int address);
int exists_in_symtab(char* symbol);
int get_byte_length(char* constant);
error delete_file(FILE* fp, char* filename, error e);
#endif
