#include "20140825.h"

int main() {
    init();
    command ret;
    while(1) {
	printf("sicsim> ");
	scanf("%[^\n]", input);
	getchar();
	flush_tokens();
	tokenize_input();
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

// flush the tokens before getting new tokens
void flush_tokens(void) {
    int i;
    for (i = 0; i < MAX_TOKENS; i++) strcpy(tokens[i], "\0");
}

// return the next token's first index in input
int get_next_token_idx(char* str) {
    int i = 0;
    if (str[i] == '\0') return -1; 			// no more token
    while (str[i] != ' ' && str[i] != '\0') i++;	// get the next token's first idx
    if (str[i] == '\0') return i;
    str[i++] = '\0'; 	                                // separate the tokens using a null char
    while (str[i] == ' ') i++;
    return i;
}

// parse the command and arguments from input, into tokens
void tokenize_input(void) {
    int start_idx = 0, next_idx = 0, token_cnt = 0;
    
    while (input[start_idx] == ' ') start_idx++;		// remove whitespaces in left
    while (start_idx < MAX_INPUT_LEN) {
	next_idx = get_next_token_idx(input + start_idx);
	if (next_idx == -1) break;				// no more tokens
	if (token_cnt >= MAX_TOKENS) break;			// check the limitations
	if (strlen(input + start_idx) > MAX_TOKEN_LEN) break;
	strcpy(tokens[token_cnt++], input + start_idx);   	// store the tokens
	start_idx += next_idx;
    }
}

command get_command(void) {
    char* cmd = tokens[0];
    if (strcmp_twice(cmd, "h", "help")) return _help;
    else if (strcmp_twice(cmd, "q", "quit")) return _quit;
    else if (strcmp_twice(cmd, "d", "dir")) return _dir;
    else if (strcmp_twice(cmd, "hi", "history")) return _history;
    else if (strcmp_twice(cmd, "du", "dump")) return _dump;
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
    case _dump:
        dump(0, 16*10);
        break;
    default:
	break;
    }
}

void init(void) {
    HEAD_LOG = NULL;
    TAIL_LOG = NULL;
    LAST_ADDR = 0;
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
