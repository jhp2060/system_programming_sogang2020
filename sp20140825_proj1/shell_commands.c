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

// print out the LOGs
void history(void) {
    node* now = HEAD_LOG;
    int count = 1;
    while (now != NULL) {
	printf("%4d %s\n", count++, now->str);
	if (count > 100) break;
	now = now->next;
    }
}

// push the commands into linked list using HEAD_LOG and TAIL_LOG
void push_log(char* string) {
    node* new = malloc(sizeof(node));
    strcpy(new->str, string);
    new->next = NULL;
    if (TAIL_LOG == NULL) {
	HEAD_LOG = new;
	TAIL_LOG = new;
    }
    else {
	TAIL_LOG->next = new;
	TAIL_LOG = new;
    }    
}

// free the memory spaces assigned to each node
void free_log() {
    node* tmp;
    while (HEAD_LOG != NULL) {
        tmp = HEAD_LOG;
        HEAD_LOG = HEAD_LOG->next;
        free(tmp);
    }
    TAIL_LOG = NULL;
}
