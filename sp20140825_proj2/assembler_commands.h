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
#define MAX_LINE_LEN 200
#define MAX_LABEL_LEN 100
#define MAX_OPCODE_LEN 100
#define MAX_OPERAND_LEN 100
#define MAX_FILENAME_LEN 50
#define MAX_OBJECT_CODE_LEN 20
#define MAX_TEXT_RECORD_LEN 30

typedef enum _linetype {
    LT_START,
    LT_END,
    LT_COMMENT,
    LT_OPCODE,
    LT_BASE,
    LT_NOBASE,
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
sym_node* RECENT_SYMTAB;
int base_register;

error assemble(char* filename, int token_count);

// functions for assemble
error pass1(FILE* fp, char* prefix, int* program_length);
error pass2(char* prefix, int program_length);

// processing strings
void read_line(FILE* fp, char* line);
linetype parse(char* line, char* label, char* opcode, char* op1, char* op2);
linetype parse2(char* line, int* locctr, char* label, char* opcode, char* op1, char* op2);

// functions for symtab
void init_symtab(void);
void free_symtab(sym_node* head);
void push_symtab(char* symbol, int address);
void preload_registers_on_symtab(void);
int exists_in_symtab(char* symbol);
sym_node* get_symbol(char* symbol);

int get_byte_length(char* constant);
error get_object_code(char* ret, int pc, char* opcode, char* op1, char* op2);
void update_text_record_len(char* text_record, int len);
int is_constant(char* symbol); 

// funcitons for cases of assembly failure
error delete_file(FILE* fp, char* filename, error e);
error assemble_failed(FILE* lstfp, FILE* objfp, char* prefix, error e);

#endif
