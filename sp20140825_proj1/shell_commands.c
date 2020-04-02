#include "shell_commands.h"

// print out help text for commands
void help(void) {
    printf("h[elp]\n");
    printf("d[ir]\n");
    printf("q[uit]\n");
    printf("hi[story]\n");
    printf("du[mp] [start, end]\n");
    printf("e[dit] address, value\n");
    printf("f[ill] start, end, value\n");
    printf("reset\n");
    printf("opcode mnemonic\n");
    printf("opcodelist\n");
}

// print out the files and directories 
void dir(void) {
    DIR *pwd = opendir(".");
    struct dirent *pwd_entry = NULL;
    struct stat entry_stat;

    if (pwd != NULL) {
	pwd_entry = readdir(pwd);
	while (pwd_entry != NULL) {
	    printf("%s", pwd_entry->d_name);
	    lstat(pwd_entry->d_name, &entry_stat);
	    if (S_IFREG & entry_stat.st_mode) {
		if (S_IXGRP & entry_stat.st_mode) printf("*");
		printf("\n");
	    }
	    else if (S_IFDIR & entry_stat.st_mode) printf("/\n");
	    pwd_entry = readdir(pwd);
	}
    }
}

// exit the shell program
void quit(void) {
    return;
}

void history(void) {
    node* now = head_log;
    int count = 1;
    while (now != NULL) {
	printf("%4d %s\n", count++, now->str);
	if (count > 100) break;
	now = now->next;
    }
}

// push the commands into linked list using head_log and tail_log
void push_log(char* string) {
    node* new = malloc(sizeof(node));
    strcpy(new->str, string);
    new->next = NULL;
    if (tail_log == NULL) {
	head_log = new;
	tail_log = new;
    }
    else {
	tail_log->next = new;
	tail_log = new;
    }    
}
