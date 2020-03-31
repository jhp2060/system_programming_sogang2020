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
    struct dirent *pwd_entry;

    if (pwd != NULL) {
	pwd_entry = readdir(pwd);
	while (pwd_entry != NULL) {
	    printf("%s\n", pwd_entry->d_name);
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
