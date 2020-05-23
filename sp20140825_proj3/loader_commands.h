#ifndef __LOADER_COMMANDS_H__
#define __LOADER_COMMANDS_H__

#include "helpers.h"
#include "memory_commands.h"
#include "assembler_commands.h"

#define MAX_SYMBOL_LEN 10
#define MAX_OBJ_LINE_LEN MAX_LINE_LEN
#define MAX_HASH_SIZE 20
#define MAX_FILES 10
#define MAX_FILE_LEN 20
#define MAX_REFERENCES 100

typedef struct _es_node{
    char name[MAX_SYMBOL_LEN];
    int addr;
    struct _es_node* next;
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

es_node* ESTAB[MAX_HASH_SIZE];

int PROGADDR;
int CSADDR;
int EXECADDR;
int CSLTH;
int TOTLTH;


bp_node* BPTAB;
int BPCHK[MEM_SIZE];

error progaddr(char* address, int token_count);
error loader(char filenames[MAX_FILES][MAX_FILE_LEN], int token_count);
error bp(char* address, int token_count);
error run(int token_count);

/* functions for EXECUTING COMMANDS */
// LOADER
void loader_pass1(FILE* fp);
void loader_pass2(FILE *fp);

// RUN
void dumpreg(void);

/* functions for DATA STRUCTURES */

// functions for BPTAB
void push_bp(int address);

// functions for ESTAB
int get_estab_hash(char* es_name);
void push_es(char *es_name, int es_addr);
es_node* get_es(char*es_name);
void init_estab();
void free_estab();

// tools
void init_registers(int l, int pc);
void get_Nbytes(char *dest, char *src, int N);
void read_line2(FILE *fp, char *line);

#endif