#ifndef __MEMORY_COMMANDS_H__
#define __MEMORY_COMMANDS_H__

#include <stdio.h>
#include <string.h>
#include "helpers.h"

#define MEM_SIZE 1048576

char MEM[MEM_SIZE];
int LAST_ADDR;
int TOKEN_COUNT;

int dump(char* start, char* end);

// functions for "dump"
void print_chars(int row);
int transform_start(char* start);
int transform_end(int start, char* end);
int validate_start_end(int start, int end);


#endif
