#include "20140825.h"

char input[100];

int main() {
    init();
    command ret;
    while(1) {
	printf("sicsim> ");
	scanf("%s", input);
	ret = get_command();
	if (ret != _none) {
	    if (ret == _quit) break;
	    push_log(input);
	    execute_instructions(ret);
	}
    }
    exit_program();
    return 0; 
}

command get_command(void) {
    if (strcmp_twice(input, "h", "help")) return _help;
    else if (strcmp_twice(input, "q", "quit")) return _quit;
    else if (strcmp_twice(input, "d", "dir")) return _dir;
    else if (strcmp_twice(input, "hi", "history")) return _history;
    return _none;
}

void execute_instructions(command c) {
    switch(c) {
    case _help:
	help();
	break;
    case _quit:
	quit();
	break;
    case _dir:
	dir();
	break;
    case _history:
	history();
	break;
    default:
	break;
    }
}

void init(void) {
    HEAD_LOG = NULL;
    TAIL_LOG = NULL;
}

void exit_program(void) {
    node* tmp;
    while (HEAD_LOG != NULL) {
	tmp = HEAD_LOG;
	HEAD_LOG = HEAD_LOG->next;
	free(tmp);
    }
    TAIL_LOG = NULL;
}
