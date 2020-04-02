#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H

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

node* head_log;
node* tail_log;

void help(void);
void dir(void);
void quit(void);
void history(void);

void push_log(char* string);

#endif
