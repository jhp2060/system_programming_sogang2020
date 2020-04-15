#ifndef __OPCODE_COMMANDS_H__
#define __OPCODE_COMMANDS_H__

#include <stdio.h>
#include <stdlib.h>
#include "helpers.h"

#define MAX_OPCODE_LEN 3
#define MAX_MNEMONIC_LEN 10
#define MAX_FORMAT_LEN 10
#define HASH_TABLE_SIZE 20

typedef struct _hash {
    char opcode[MAX_OPCODE_LEN];
    char mnemonic[MAX_MNEMONIC_LEN];
    char format[MAX_FORMAT_LEN];
    struct _hash *next;
}hash;

hash* HASH_TABLE[HASH_TABLE_SIZE];

int opcode(char* mnemonic, int token_count);
void opcodelist(void);

// functions about hashing
void init_hash_table(char* filename);
int get_hash_index(char* mnemonic);
void free_hash_table(void);

#endif
