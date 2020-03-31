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
    if (strcmp(input, "h") == 0 || strcmp(input, "help") == 0) {
	help();
	return _help;
    }
    else if (strcmp(input, "q") == 0 || strcmp(input, "quit") == 0) {
 	quit();
	return _quit;
    }
    return _none;
}
