#include "loader_commands.h"

error progaddr(char *address, int token_count) {
    if (token_count != 2) return ERR_WRONG_TOKENS;

    int addr = -1;
    error e;

    e = hexstr_to_int(address, &addr);
    if (e != NO_ERR) return e;

    e = validate_address(addr);
    if (e != NO_ERR) return e;

    PROGADDR = addr;
    return NO_ERR;
}

error loader(char filenames[MAX_FILES][MAX_FILE_LEN], int token_count) {
    if (token_count < 2) return ERR_WRONG_TOKENS;

    int i = 1;
    char *objfile = filenames[i++];
    FILE *objfp;

    TOTLTH = 0;

    // print loadmap
    printf("%7-s  %7-s  %7-s  %7-s\n", "control", "symbol", "address", "length");
    printf("%7-s  %7-s\n", "section", "name");
    printf("-----------------------------------\n");
    while (!is_nullstr(objfile)) {
        objfp = fopen(objfile, "r");
        if (!objfp) return ERR_NOT_A_FILE;

        loader_pass1(objfp);

        fclose(objfp);
        objfile = filenames[i++];
    }
    printf("-----------------------------------\n");
    printf("%7-s  %7-s  %7-s  %04X   \n", " ", "  total", "length", TOTLTH);

    init_registers(TOTLTH, PROGADDR);

    // TODO pass2

    return NO_ERR;
}

error bp(char *address, int token_count) {
    if (token_count == 1) {
        bp_node *now = BPTAB;
        printf("\t\tbreakpoint\n");
        printf("\t\t----------\n");
        while (now) {
            printf("\t\t%X\n", now->addr);
            now = now->next;
        }
    } else if (token_count == 2) {
        int addr = -1;
        error e;

        if (strcmp(address, "clear") == 0) {
            bp_node *now = BPTAB;
            bp_node *tmp = NULL;
            while (now) {
                BPCHK[now->addr] = 0;
                tmp = now;
                now = now->next;
                free(tmp);
            }
            BPTAB = NULL;
            printf("\t\t[ok] clear all breakpoints\n");
        } else {
            e = hexstr_to_int(address, &addr);
            if (e != NO_ERR) return e;

            e = validate_address(addr);
            if (e != NO_ERR) return e;

            push_bp(addr);
            BPCHK[addr] = 1;
            printf("\t\t[ok] create breakpoint %s\n", address);
        }
    } else return ERR_WRONG_TOKENS;

    return NO_ERR;
}


error run(int token_count) {
    if (token_count != 1) return ERR_WRONG_TOKENS;

    bp_node *now = BPTAB;

    // TODO run the program

    printf("A : %06X   X : %06X\n", reg_A, reg_X);
    printf("L : %06X  PC : %06X\n", reg_L, reg_PC);
    printf("B : %06X   S : %06X\n", reg_B, reg_S);
    printf("T : %06X\n", reg_T);

    // TODO whether bp exists or not

    return NO_ERR;
}

/* functions for EXECUTING COMMANDS */

// pass1 for loader : assign addresses to all external symbols
void loader_pass1(FILE *fp) {
    char *chptr;
    char line[MAX_OBJ_LINE_LEN];
    char word[MAX_OBJ_LINE_LEN];
    char tmp1[MAX_OBJ_LINE_LEN];
    char tmp2[MAX_OBJ_LINE_LEN];
    int address;

    CSADDR = PROGADDR;

    read_line(fp, line);
    while (!feof(fp)) {
        switch (line[0]){
            case 'H':     // H record
                // get control section name
                chptr = strtok(line, " ");
                strcpy(tmp1, chptr + 1);

                // get starting address of object program
                chptr = strtok(NULL, " ");
                get_3bytes(tmp2, chptr);
                hexstr_to_int(tmp2, &address);

                push_es(tmp1, CSADDR + address);

                // get the length of object program in bytes
                get_3bytes(tmp2, chptr + 6);
                hexstr_to_int(tmp2, &CSLTH);

                TOTLTH += CSLTH;
                printf("%7-s  %7-s  %04X     %04X   \n", tmp1, " ", CSADDR + address, CSLTH);

                break;
            case 'D':   // D record
                // get the first external symbol's name
                chptr = strtok(line, " ");
                chptr++;
                strcpy(tmp1, chptr);

                while (1) {
                    // get the address of external symbol
                    chptr = strtok(NULL, " ");
                    get_3bytes(tmp2, chptr);
                    hexstr_to_int(tmp2, &address);

                    push_es(tmp1, address + CSADDR);

                    printf("%7-s  %7-s  %04X   \n", " ", tmp1, CSADDR + address );

                    if (*(chptr + 6) == '\0') break;

                    // get the name of a external symbol
                    strcpy(tmp1, chptr + 6);
                }

                break;
            case 'E':
                CSADDR += CSLTH;
                break;
            default:
                break;
        }
        read_line(fp, line);
    }
}

void loader_pass2() {

}


// push a break point into BPTAB
void push_bp(int address) {
    bp_node *now = BPTAB;
    bp_node *new = malloc((sizeof(bp_node)));
    new->addr = address;
    new->next = NULL;
    if (BPTAB == NULL) {
        BPTAB = new;
        return;
    }
    while (now->next) now = now->next;
    now->next = new;
}


/* functions for ESTAB */

// hash function for es_node in ESTAB
int get_estab_hash(char *es_name) {
    int i, ret = 0, len = strlen(es_name);
    for (i = 0; i < len; i++) ret += es_name[i];
    if (ret < 0) ret *= -1;
    return ret % MAX_HASH_SIZE;
}

// push an es_node into ESTAB
void push_es(char *es_name, int es_addr) {
    int index = get_estab_hash(es_name);
    es_node *now = ESTAB[index];
    es_node *to_push = malloc(sizeof(es_node));

    strcpy(to_push->name, es_name);
    to_push->addr = es_addr;
    to_push->next = NULL;

    if (now == NULL){
        ESTAB[index] = now;
        return;
    }
    while (now->next) {
        if (strcmp(now->name, es_name) == 0){
            free(to_push);
            return;
        }
        now = now->next;
    }
    now->next = to_push;
}

// get es_node from ESTAB with es_name
es_node *get_es(char *es_name) {
    es_node *now = ESTAB[get_estab_hash(es_name)];
    while (now) {
        if (strcmp(now->name, es_name) == 0)
            return now;
        now = now->next;
    }
    return NULL;
}

// initialize ESTAB
void init_estab() {
    int i = 0;
    for (i = 0; i < MAX_HASH_SIZE; i++) ESTAB[i] = NULL;
}

// free the allocated memories
void free_estab() {
    int i = 0;
    es_node *now, *tmp;

    for (i = 0; i < MAX_HASH_SIZE; i++) {
        now = ESTAB[i];
        while (now) {
            tmp = now;
            now = now->next;
            free(now);
        }
        ESTAB[i] = NULL;
    }
}



// initialize the values of registers
void init_registers(int l, int pc) {
    reg_A = 0;
    reg_X = 0;
    reg_L = l;
    reg_PC = pc;
    reg_B = 0;
    reg_S = 0;
    reg_T = 0;
}

void get_3bytes(char* dest, char* src){
    strncpy(dest, src, 6);
    dest[6] = '\0';
}