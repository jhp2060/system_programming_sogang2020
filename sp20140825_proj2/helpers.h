#ifndef HELPERS_H
#define HELPERS_H

#include <string.h>

// define the error codes
#define NO_ERR 1
#define ERR_WRONG_HEXSTR -1
#define ERR_ARGS_FORMAT -2
#define ERR_WRONG_ADDR -3
#define ERR_WRONG_VAL -4
#define ERR_WRONG_RANGE -5
#define ERR_WRONG_TOKENS -6
#define ERR_WRONG_MNEMONIC -7

int strcmp_twice(char* norm, char* op1, char* op2);
int hexstr_to_int(char* str);

#endif
