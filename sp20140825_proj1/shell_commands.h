#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "helpers.h"

void help(void);
void dir(void);
void quit(void);
void history(void);

#endif
