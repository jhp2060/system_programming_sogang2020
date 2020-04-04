#ifndef __OPCODE_COMMANDS_H__
#define __OPCODE_COMMANDS_H__

#include <stdio.h>
#include "helpers.h"

#define MAX_OPCODE_LEN 3
#define MAX_MNEUMONIC_LEN 10
#define MAX_FORMAT_LEN 10
#define HASH_TABLE_SIZE 20

typedef struct _hash {
    char opcode[MAX_OPCODE_LEN];
    char mneumonic[MAX_MNEUMONIC_LEN];
    char format[MAX_FORMAT_LEN];
    struct _hash *next;
}hash;

hash* HASH_TABLE[HASH_TABLE_SIZE];

void opcode(char* mnemonic, int token_count);
void opcodelist();

// functions about hashing
void init_hash_table(char* filename);
int get_hash_index(hash* node);
void free_hash_table(void);

#endif
