#ifndef HELPERS_H
#define HELPERS_H

#include <string.h>
#include <stdio.h>

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
}error;

int strcmp_twice(char* norm, char* op1, char* op2);
int hexstr_to_int(char* str);
void print_error_msg(error e);

#endif
