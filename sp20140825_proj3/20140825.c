#include "20140825.h"

int main() {
    char log_sentence[100];
    error e;
    init();
    command ret;
    while (1) {
        flush_input();
        printf("sicsim> ");
        scanf("%[^\n]", input);
        strcpy(log_sentence, input);        // copy for push_log
        getchar();                // flush the buffer
        tokenize_input();
        ret = get_command();
        e = execute_instructions(ret);
        if (ret == CMD_QUIT && e == NO_ERR) break;
        if (e == NO_ERR) // do not make wrong commands in the log
            push_log(log_sentence);
        else
            print_error_msg(e);
    }
    exit_program();
    return 0;
}

void flush_input(void) {
    input[0] = '\0';
}

// flush the tokens before getting new tokens
void flush_tokens(void) {
    int i;
    for (i = 0; i < MAX_TOKENS; i++) strcpy(tokens[i], "\0");
    TOKEN_COUNT = 0;
}

// return the input index of next token's first char
int get_next_token_idx(char *str) {
    int i = 0;
    if (str[i] == '\0') return -1;            // no more token
    while (str[i] != ' ' && str[i] != '\t' && str[i] != '\0') i++;    // get the next token's first idx
    if (str[i] == '\0') return i;
    str[i++] = '\0';                                    // separate the tokens using a null char
    while (str[i] == ' ' || str[i] == '\t') i++;
    return i;
}

// parse the command and arguments from input, into tokens
void tokenize_input(void) {
    int start_idx = 0, next_idx = 0;
    flush_tokens();

    while (input[start_idx] == ' ' || input[start_idx] == '\t') start_idx++;        // remove whitespaces in left
    while (start_idx < MAX_INPUT_LEN) {
        next_idx = get_next_token_idx(input + start_idx);
        if (next_idx == -1) return;                // no more tokens
        if (TOKEN_COUNT >= MAX_TOKENS) return;            // check the limitations
        if (strlen(input + start_idx) > MAX_TOKEN_LEN) return;
        strcpy(tokens[TOKEN_COUNT++], input + start_idx);    // store the tokens
        start_idx += next_idx;
    }
}

// return the command according to input and tokens
command get_command(void) {
    char *cmd = tokens[0];
    if (strcmp_twice(cmd, "h", "help")) return CMD_HELP;
    else if (strcmp_twice(cmd, "q", "quit")) return CMD_QUIT;
    else if (strcmp_twice(cmd, "d", "dir")) return CMD_DIR;
    else if (strcmp_twice(cmd, "hi", "history")) return CMD_HISTORY;
    else if (strcmp_twice(cmd, "du", "dump")) return CMD_DUMP;
    else if (strcmp_twice(cmd, "e", "edit")) return CMD_EDIT;
    else if (strcmp_twice(cmd, "f", "fill")) return CMD_FILL;
    else if (strcmp(cmd, "reset") == 0) return CMD_RESET;
    else if (strcmp(cmd, "opcode") == 0) return CMD_OPCODE;
    else if (strcmp(cmd, "opcodelist") == 0) return CMD_OPCODELIST;
    else if (strcmp(cmd, "assemble") == 0) return CMD_ASSEMBLE;
    else if (strcmp(cmd, "type") == 0) return CMD_TYPE;
    else if (strcmp(cmd, "symbol") == 0) return CMD_SYMBOL;
    else if (strcmp(cmd, "progaddr") == 0) return CMD_PROGADDR;
    else if (strcmp(cmd, "loader") == 0) return CMD_LOADER;
    else if (strcmp(cmd, "bp") == 0) return CMD_BP;
    else if (strcmp(cmd, "run") == 0) return CMD_RUN;
    else return CMD_NONE;
}

// exectue the instructions according to the command
error execute_instructions(command c) {
    error e;
    switch (c) {
        case CMD_HELP:
            e = help(TOKEN_COUNT);
            break;
        case CMD_QUIT:
            e = quit(TOKEN_COUNT);
            break;
        case CMD_DIR:
            e = dir(TOKEN_COUNT);
            break;
        case CMD_HISTORY:
            if (TOKEN_COUNT == 1)
                push_log(tokens[0]);
            e = history(TOKEN_COUNT);
            break;
        case CMD_DUMP:
            e = dump(tokens[1], tokens[2], TOKEN_COUNT);
            break;
        case CMD_EDIT:
            e = edit(tokens[1], tokens[2], TOKEN_COUNT);
            break;
        case CMD_FILL:
            e = fill(tokens[1], tokens[2], tokens[3], TOKEN_COUNT);
            break;
        case CMD_RESET:
            e = reset(TOKEN_COUNT);
            break;
        case CMD_OPCODE:
            e = opcode(tokens[1], TOKEN_COUNT);
            break;
        case CMD_OPCODELIST:
            e = opcodelist(TOKEN_COUNT);
            break;
        case CMD_ASSEMBLE:
            e = assemble(tokens[1], TOKEN_COUNT);
            break;
        case CMD_TYPE:
            e = type(tokens[1], TOKEN_COUNT);
            break;
        case CMD_SYMBOL:
            e = symbol(TOKEN_COUNT);
            break;
        case CMD_PROGADDR:
            e = progaddr(tokens[1], TOKEN_COUNT);
            break;
        case CMD_LOADER:
            e = loader(tokens, TOKEN_COUNT);
            break;
        case CMD_BP:
            e = bp(tokens[1], TOKEN_COUNT);
            break;
        case CMD_RUN:
            e = run(TOKEN_COUNT);
            break;
        default:
            e = ERR_WRONG_COMMAND;
            break;
    }
    return e;
}

// initialize global variables and hash table
void init(void) {
    HEAD_LOG = NULL;
    TAIL_LOG = NULL;
    TOKEN_COUNT = 0;
    LAST_ADDR = -1;
    init_optab(OPCODE_FILENAME);
    init_symtab();
    RECENT_SYMTAB = NULL;
    PROGADDR = 0;
    BPTAB = NULL;
    init_estab();
}

// free the dynamically allocated memories
void exit_program(void) {
    free_log();
    free_optab();
    free_symtab(SYMTAB);
    free_symtab(RECENT_SYMTAB);
    // free_bp();
}
