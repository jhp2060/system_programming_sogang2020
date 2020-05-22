#ifndef __LOADER_COMMANDS_H__
#define __LOADER_COMMANDS_H__

#include "helpers.h"
#include "memory_commands.h"
#include "assembler_commands.h"

#define MAX_SYMBOL_LEN 10
#define MAX_OBJ_LINE_LEN MAX_LINE_LEN

typedef struct _es_node{
    char* name[MAX_SYMBOL_LEN];
    int address;
    int csec;
}es_node;

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

es_node* ESTAB;
int PROGADDR;
int CSADDR;

bp_node* BP_HEAD;
int BPCHK[MEM_SIZE];

error progaddr(char* address, int token_count);
error loader(char** filenames, int token_count);
error bp(char* address, int token_count);
error run(int token_count);

void loader_pass1(FILE* fp);
void loader_pass2();

void push_bp(int address);
void init_registers(int pc, int l);

#endif