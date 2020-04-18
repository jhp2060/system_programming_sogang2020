#ifndef __OPCODE_COMMANDS_H__
#define __OPCODE_COMMANDS_H__

#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

#define MAX_MNEMONIC_LEN 10
#define MAX_FORMAT_LEN 10
#define OPTAB_SIZE 20

typedef struct _opcode_node{
    char opcode[3];
    char mnemonic[MAX_MNEMONIC_LEN];
    char format[MAX_FORMAT_LEN];
    struct _opcode_node* next;
}opcode_node;

opcode_node* OPTAB[OPTAB_SIZE];

error opcode(char* mnemonic, int token_count);
error opcodelist(int token_count);

// functions about hashing
void init_optab(char* filename);
int get_optab_index(char* mnemonic);
void free_optab(void);
opcode_node* get_opcode(char* mnemonic);
#endif
