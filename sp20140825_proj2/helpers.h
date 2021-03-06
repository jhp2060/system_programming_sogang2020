#ifndef HELPERS_H
#define HELPERS_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// define the error codes
typedef enum _error {
    NO_ERR=0,
    NO_ERR_HISTORY,
    ERR_WRONG_COMMAND,
    ERR_WRONG_HEXSTR,
    ERR_ARGS_FORMAT,
    ERR_WRONG_ADDR,
    ERR_WRONG_VAL,
    ERR_WRONG_RANGE,
    ERR_WRONG_TOKENS,
    ERR_WRONG_MNEMONIC,
    ERR_NOT_A_FILE,
    ERR_NOT_A_ASM_FILE,
    ERR_NO_END,
    ERR_SYMBOL_DUPLICATED,
    ERR_NO_INTERMEDIATE_FILE,
    ERR_NO_INST_OPERAND,
    ERR_NO_SYMBOL,
    ERR_NO_INST_COMMA,
}error;

int strcmp_twice(char* norm, char* op1, char* op2);
error hexstr_to_int(char* str, int* ret);
void print_error_msg(error e);
int is_nullstr(char* str);
void int_to_binstr(char* ret, int val, int digit);

#endif
