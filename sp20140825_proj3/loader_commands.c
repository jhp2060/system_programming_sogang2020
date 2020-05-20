#include "loader_commands.h"


error progaddr(char *address, int token_count) {
    if (token_count != 2) return ERR_WRONG_TOKENS;

    int addr = -1;
    hexstr_to_int(address, addr);
    PROG_ADDR = addr;
    return NO_ERR;
}

error loader(char **filenames, int token_count) {
    if (token_count < 2) return ERR_WRONG_TOKENS;

    char *objfile;

    int i = 0;
    while (objfile) {
        objfile = filenames[i];

    }

    return NO_ERR;
}

error bp(char *address, int token_count) {
    int addr = -1;
    hexstr_to_int(address, addr);

    if (token_count == 1) {
        bp_node *now = BP_HEAD;
        printf("\t\tbreakpoint\n");
        printf("\t\t----------\n");
        while (now) {
            printf("\t\t%d\n", now->addr);
            now = now->next;
        }
    } else if (token_count == 2) {
        if (strcmp(address, "clear") == 0) {
            printf("\t\t[ok] clear all breakpoints\n");
        } else {
            push_bp(addr);
            printf("\t\t[ok] create breakpoint %04X\n", addr);
        }
    } else return ERR_WRONG_TOKENS;

    return NO_ERR;
}


error run(int token_count) {
    if (token_count != 1) return ERR_WRONG_TOKENS;

    bp_node *now = BP_HEAD;

    // TODO run the program

    printf("A : %06X   X : %06X\n", reg_A, reg_X);
    printf("L : %06X  PC : %06X\n", reg_L, reg_PC);
    printf("B : %06X   S : %06X\n", reg_B, reg_S);
    printf("T : %06X\n", reg_T);

    // TODO whether bp exists or not

    return NO_ERR;
}


void push_bp(int address) {
    bp_node *now = BP_HEAD;
    bp_node *new = malloc((sizeof(bp_node)));
    new->addr = address;
    new->next = NULL;
    if (BP_HEAD == NULL) {
        BP_HEAD = new;
        return;
    }
    while (now->next) now = now->next;
    now->next = new;
}