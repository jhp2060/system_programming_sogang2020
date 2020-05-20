#ifndef __MEMORY_COMMANDS_H__
#define __MEMORY_COMMANDS_H__

#include <stdio.h>
#include <string.h>
#include "helpers.h"

#define MEM_SIZE 1048576

char MEM[MEM_SIZE];
int LAST_ADDR;

error dump(char* start, char* end, int token_count);
error edit(char* address, char* value, int token_count);
error fill(char* start, char* end, char* value, int token_count);
error reset(int token_count);

// functions for "dump"
void print_chars(int row);

// functions for argument validation
error validate_one_hexstr_argument(char* arg1, int* ret1);
error validate_two_hexstr_arguments(char* arg1, char* arg2, int* ret1, int* ret2);
error validate_three_hexstr_arguments(
    char* arg1, char* arg2, char* arg3, int* ret1, int* ret2, int* ret3);
error validate_address(int address);
error validate_value(int value);
error validate_range(int start, int end);

#endif
