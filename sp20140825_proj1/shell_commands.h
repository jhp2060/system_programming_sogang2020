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

node* HEAD_LOG;
node* TAIL_LOG;

void help(void);
void dir(void);
void quit(void);
void history(void);

void push_log(char* string);
#endif
