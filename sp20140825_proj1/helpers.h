#ifndef HELPERS_H
#define HELPERS_H

#include <string.h>

// define the error codes
typedef enum _error {
    NO_ERR = 1,
    ERR_WRONG_HEXSTR,
    ERR_ARGS_FORMAT,
    ERR_WRONG_ADDR,
    ERR_WRONG_VAL,
    ERR_WRONG_RANGE,
    ERR_WRONG_TOKENS
}error;

int strcmp_twice(char* norm, char* op1, char* op2);
int hexstr_to_int(char* str);

#endif
