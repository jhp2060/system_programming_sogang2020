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


void dir(void) {
    DIR *pwd = opendir(".");
    struct dirent *pwd_entry = NULL;
    struct stat entry_stat;

    if (pwd != NULL) {
	pwd_entry = readdir(pwd);
	while (pwd_entry != NULL) {
	    /*
	    if (strcmp_twice(pwd_entry->d_name, ".", "..")) {
		pwd_entry = readdir(pwd);
		continue;
	    }
		*/
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

}
