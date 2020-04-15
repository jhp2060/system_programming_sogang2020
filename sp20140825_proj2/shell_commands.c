#include "shell_commands.h"

// print out help text for commands
error help(int token_count) {
    if (token_count != 1) return ERR_WRONG_TOKENS;
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
    return NO_ERR;
}

// print out the files and directories 
error dir(int token_count) {
    if (token_count != 1) return ERR_WRONG_TOKENS;
    DIR *pwd = opendir("."); 		// get the present working directory
    struct dirent *pwd_entry = NULL;
    struct stat entry_stat;

    if (pwd != NULL) {
	pwd_entry = readdir(pwd);	// get the entry of pwd
	while (pwd_entry != NULL) {
	    printf("%s", pwd_entry->d_name);
	    lstat(pwd_entry->d_name, &entry_stat);		// store the status of the entry
	    if (S_IFREG & entry_stat.st_mode) {			// regular file
		if (S_IXUSR & entry_stat.st_mode || S_IXGRP & entry_stat.st_mode
		    || S_IXOTH & entry_stat.st_mode) 		// execution file
		    printf("*");	
		printf("\n");
	    }
	    else if (S_IFDIR & entry_stat.st_mode)		// directory
		printf("/\n"); 	
	    pwd_entry = readdir(pwd);
	}
    }
    return NO_ERR;
}

// exit the shell program
error quit(int token_count) {
    if (token_count != 1) return ERR_WRONG_TOKENS;
    return NO_ERR;
}

// print out the LOGs
error history(int token_count) {
    if (token_count != 1) return ERR_WRONG_TOKENS;
    node* now = HEAD_LOG;
    int count = 1;
    while (now != NULL) {
	printf("%4d %s\n", count++, now->str);
	now = now->next;
    }
    return NO_ERR_HISTORY;
}

// push the commands into linked list(log) using HEAD_LOG and TAIL_LOG
void push_log(char* string) {
    node* new = malloc(sizeof(node));
    strcpy(new->str, string);
    new->next = NULL;
    if (TAIL_LOG == NULL) {	// initialize
	HEAD_LOG = new;
	TAIL_LOG = new;
    }
    else {
	TAIL_LOG->next = new;
	TAIL_LOG = new;
    }    
}

// free the memory spaces assigned to each node
void free_log(void) {
    node* tmp;
    while (HEAD_LOG != NULL) {
        tmp = HEAD_LOG;
        HEAD_LOG = HEAD_LOG->next;
        free(tmp);
    }
    TAIL_LOG = NULL;
}
