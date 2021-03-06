#ifndef __MEMORY_COMMANDS_H__
#define __MEMORY_COMMANDS_H__

#include <stdio.h>
#include <string.h>
#include "helpers.h"

#define MEM_SIZE 1048576

char MEM[MEM_SIZE];
int LAST_ADDR;

int dump(char* start, char* end, int token_count);
int edit(char* address, char* value, int token_count);
int fill(char* start, char* end, char* value, int token_count);
void reset(void);

// functions for "dump"
void print_chars(int row);

// functions for argument validation
int validate_one_hexstr_argument(char* arg1, int* ret1);
int validate_two_hexstr_arguments(char* arg1, char* arg2, int* ret1, int* ret2);
int validate_three_hexstr_arguments(
    char* arg1, char* arg2, char* arg3, int* ret1, int* ret2, int* ret3);
int validate_address(int address);
int validate_value(int value);
int validate_range(int start, int end);

#endif
