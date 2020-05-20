#ifndef __SHELL_COMMANDS_H__
#define __SHELL_COMMANDS_H__

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "helpers.h"

typedef struct _node {
    char str[100];
    struct _node* next;
}node;

node* HEAD_LOG; // points the head of log
node* TAIL_LOG; // points the tail of log

error help(int token_count);
error dir(int token_count);
error quit(int token_count);
error history(int token_count);
error type(char* filename, int token_count);

void push_log(char* string);
void free_log(void);
#endif
