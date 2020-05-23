#ifndef __OPCODE_COMMANDS_H__
#define __OPCODE_COMMANDS_H__

#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

#define MAX_MNEMONIC_LEN 10
#define MAX_FORMAT_LEN 10
#define OPTAB_SIZE 20

typedef struct _opcode_node{
    int opcode;
    char mnemonic[MAX_MNEMONIC_LEN];
    char format[MAX_FORMAT_LEN];
    struct _opcode_node* next;
    struct _opcode_node* next2;
}opcode_node;

opcode_node* OPTAB[OPTAB_SIZE];
opcode_node* OPTAB2[OPTAB_SIZE];

error opcode(char* mnemonic, int token_count);
error opcodelist(int token_count);

// functions about hashing
void init_optab(char* filename);
int get_optab_index(char* mnemonic);
int get_optab_index2(int opcode);
void free_optab(void);
opcode_node* get_opcode(char* mnemonic);
opcode_node* get_opcode2(int opcode);
#endif
