#ifndef __ASSEMBLE_COMMANDS_H__
#define __ASSEMBLE_COMMANDS_H__

#include "helpers.h"
#include <string.h>
#include <stdio.h>

#define LINE_MULTIPLIER 5
#define MAX_LINE_LEN 30 + 1

error assemble(char* filename, int token_count);


// functions for assemble
error pass1(FILE* fp);
#endif
