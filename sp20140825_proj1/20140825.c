#include "20140825.h"

char input[100];

int main() {
    command ret;
    while(1) {
	printf("sicsim> ");
	scanf("%s", input);
	ret = process_instructions();
	if (ret == _quit) break;
	else if (ret == _none) printf("WRONG COMMANDS!\n");
    }
    return 0; 
}

command process_instructions(void) {
    if (strcmp_twice(input, "h", "help")) {
	help();
	return _help;
    }
    else if (strcmp_twice(input, "q", "quit")) {
 	quit();
	return _quit;
    }
    else if (strcmp_twice(input, "d", "dir")) {
	dir();
	return _dir;
    }
    return _none;
}

