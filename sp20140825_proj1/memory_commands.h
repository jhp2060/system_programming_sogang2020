#ifndef MEMORY_COMMANDS_H
#define MEMORY_COMMANDS_H

#include <stdio.h>

char MEM[65536][16];
int LAST_ADDR = 0;

void dump(int start, int end);

void print_address(int row);
void print_bytes(int row, int st, int ed);
void print_chars(int row);

#endif
