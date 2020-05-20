#ifndef __LOADER_COMMANDS_H__
#define __LOADER_COMMANDS_H__

#include "helpers.h"

typedef struct _bp_node{
    int addr;
    struct _bp_node* next;
}bp_node;

int reg_A;
int reg_X;
int reg_L;
int reg_PC;
int reg_B;
int reg_S;
int reg_T;
int PROG_ADDR;
bp_node* BP_HEAD;

error progaddr(char* address, int token_count);
error loader(char** filenames, int token_count);
error bp(char* address, int token_count);
error run(int token_count);

void push_bp(int address);


#endif