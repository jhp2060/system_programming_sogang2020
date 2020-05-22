#include "loader_commands.h"

error progaddr(char *address, int token_count) {
    if (token_count != 2) return ERR_WRONG_TOKENS;

    int addr = -1;
    error ret;
    hexstr_to_int(address, &addr);
    ret = validate_address(addr);
    if (ret != NO_ERR) return ret;
    PROGADDR = addr;
    return ret;
}

error loader(char **filenames, int token_count) {
    if (token_count < 2) return ERR_WRONG_TOKENS;

    int i = 0;
    char *objfile = filenames[i++];
    FILE *objfp;

    while (!is_nullstr(objfile)) {
        objfp = fopen(objfile, "r");

        loader_pass1(objfp);

        fclose(objfp);
        objfile = filenames[i++];
    }

    // TODO pring loadmap

    return NO_ERR;
}

error bp(char *address, int token_count) {
    if (token_count == 1) {
        bp_node *now = BP_HEAD;
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
            bp_node *now = BP_HEAD;
            bp_node *tmp = NULL;
            while (now) {
                BPCHK[now->addr] = 0;
                tmp = now;
                now = now->next;
                free(tmp);
            }
            BP_HEAD = NULL;
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

    bp_node *now = BP_HEAD;

    // TODO run the program

    printf("A : %06X   X : %06X\n", reg_A, reg_X);
    printf("L : %06X  PC : %06X\n", reg_L, reg_PC);
    printf("B : %06X   S : %06X\n", reg_B, reg_S);
    printf("T : %06X\n", reg_T);

    // TODO whether bp exists or not

    return NO_ERR;
}


void loader_pass1(FILE *fp) {
    char *chptr;
    char line[MAX_OBJ_LINE_LEN];
    read_line(fp, line);


}

void loader_pass2() {

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

void init_registers(int pc, int l) {
    reg_A = 0;
    reg_X = 0;
    reg_L = l;
    reg_PC = pc;
    reg_B = 0;
    reg_S = 0;
    reg_T = 0;
}