#include "20140825.h"

int main() {
    char log_sentence[100];
    init();
    command ret;
    while(1) {
	printf("sicsim> ");
	scanf("%[^\n]", input);
	strcpy(log_sentence, input);
	getchar();
	tokenize_input();
	ret = get_command();
	if (ret != _none) {
	    if (ret == _quit) break;
	    if (execute_instructions(ret) == 1)
		push_log(log_sentence);
	}
    }
    exit_program();
    return 0; 
}

// flush the tokens before getting new tokens
void flush_tokens(void) {
    int i;
    for (i = 0; i < MAX_TOKENS; i++) strcpy(tokens[i], "\0"); 
    TOKEN_COUNT = 0;
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
    int start_idx = 0, next_idx = 0;
    flush_tokens();
    
    while (input[start_idx] == ' ') start_idx++;		// remove whitespaces in left
    while (start_idx < MAX_INPUT_LEN) {
	next_idx = get_next_token_idx(input + start_idx);
	if (next_idx == -1) break;				// no more tokens
	if (TOKEN_COUNT >= MAX_TOKENS) break;			// check the limitations
	if (strlen(input + start_idx) > MAX_TOKEN_LEN) break;
	strcpy(tokens[TOKEN_COUNT++], input + start_idx);   	// store the tokens
	start_idx += next_idx;
    }
}

command get_command(void) {
    if (strcmp_twice(input, "h", "help")) return _help;
    else if (strcmp_twice(input, "q", "quit")) return _quit;
    else if (strcmp_twice(input, "d", "dir")) return _dir;
    else if (strcmp_twice(input, "hi", "history")) return _history;
    else if (strcmp_twice(tokens[0], "du", "dump")) return _dump;
    return _none;
}

int execute_instructions(command c) {
    int succeeded = 1;
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
	push_log(tokens[0]);
	history();
	succeeded = 0;
	break;
    case _dump:
	if (TOKEN_COUNT > 3) {
	    succeeded = 0;
	    break;	
	}
        if(dump(tokens[1], tokens[2]) != 1) 
	    succeeded = 0;
        break;
    default:
	succeeded = 0;
	break;
    }
    return succeeded;
}

void init(void) {
    HEAD_LOG = NULL;
    TAIL_LOG = NULL;
    TOKEN_COUNT = 0;
    LAST_ADDR = -1;
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
