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
    CSADDR = PROGADDR;

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

    // TODO pass2

    init_registers(TOTLTH, PROGADDR);

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

// pass1 for loader: assign addresses to all external symbols
void loader_pass1(FILE *fp) {
    char *chptr;
    char line[MAX_OBJ_LINE_LEN];
    char tmp1[MAX_OBJ_LINE_LEN];
    char tmp2[MAX_OBJ_LINE_LEN];
    int address;

    read_line(fp, line);
    while (!feof(fp)) {
        switch (line[0]) {
            case 'H':     // H record
                // get control section name
                chptr = strtok(line, " ");
                chptr++;
                if (strlen(chptr) > 6) {
                    strncpy(tmp1, chptr, 6);
                    tmp1[6] = '\0';
                    chptr += 6;
                } else {
                    strcpy(tmp1, chptr);
                    chptr = strtok(NULL, " ");
                }

                // get starting address of object program
                get_Nbytes(tmp2, chptr, 3);
                hexstr_to_int(tmp2, &address);

                push_es(tmp1, CSADDR + address);

                // get the length of object program in bytes
                get_Nbytes(tmp2, chptr + 6, 3);
                hexstr_to_int(tmp2, &CSLTH);

                TOTLTH += CSLTH;
                printf("%7-s  %7-s  %04X     %04X   \n", tmp1, " ", CSADDR + address, CSLTH);

                break;
            case 'D':   // D record
                // skip 'D'
                chptr = strtok(line, " ");
                chptr++;

                while (1) {
                    // get the name of external symbol
                    if (strlen(chptr) > 6) {
                        strncpy(tmp1, chptr, 6);
                        tmp1[6] = '\0';
                        chptr += 6;
                    } else {
                        strcpy(tmp1, chptr);
                        chptr = strtok(NULL, " ");
                    }

                    // get the address of external symbol
                    get_Nbytes(tmp2, chptr, 3);
                    hexstr_to_int(tmp2, &address);

                    push_es(tmp1, address + CSADDR);

                    printf("%7-s  %7-s  %04X   \n", " ", tmp1, CSADDR + address);

                    chptr += 6;
                    if (*(chptr) == '\0') break;
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

// pass2 for loader: performs the actual loading, relocation, and linking
void loader_pass2(FILE *fp) {
    char *chptr;
    char line[MAX_OBJ_LINE_LEN];
    char tmp1[MAX_OBJ_LINE_LEN];
    char tmp2[MAX_OBJ_LINE_LEN];
    int address;

    int rftab[MAX_REFERENCES];
    int rfidx;

    int treclim, byteval;

    // M record
    int halfbytenum, deltaaddr;
    es_node *es;
    int org_addr, new_addr;

    CSADDR = PROGADDR;

    read_line(fp, line);
    while (!feof(fp)) {
        switch (line[0]) {
            case 'H':     // H record
                // get control section name
                chptr = strtok(line, " ");
                strcpy(tmp1, chptr + 1);

                es_node *csec = get_es(tmp1);
                rftab[1] = csec->addr;
                break;
            case 'R':
                chptr = strtok(line, " ");
                chptr++;

                while (chptr) {
                    // get the address of external symbol
                    get_Nbytes(tmp1, chptr, 1);
                    if (hexstr_to_int(tmp1, &rfidx) == NO_ERR) {
                        strcpy(tmp2, chptr + 2);
                        es_node *es = get_es(tmp2);
                        rftab[rfidx] = es->addr;
                    }
                    chptr = strtok(NULL, " ");
                }
                break;
            case 'T':     // T record
                // get starting address of object code in a record
                chptr = strtok(line, " ");
                chptr++;
                get_Nbytes(tmp1, chptr, 3);
                hexstr_to_int(tmp1, &reg_PC); // set PC
                reg_PC += CSADDR;
                chptr += 6;

                // get length of object code
                get_Nbytes(tmp1, chptr, 1);
                hexstr_to_int(tmp2, &treclim);
                treclim += reg_PC;
                chptr += 2;

                // load byte by byte on the MEM
                while (reg_PC <= treclim) {
                    get_Nbytes(tmp1, chptr++, 1);
                    hexstr_to_int(tmp1, &byteval);
                    if (validate_value(byteval) == NO_ERR) {
                        printf("ERROR: wrong value to store!!!\n");
                        return;
                    }
                    MEM[reg_PC++] = (char) byteval;
                }
                break;
            case 'M':   // M record
                // get starting location of the address field to be modified
                chptr = line + 1;
                get_Nbytes(tmp1, chptr, 3);
                hexstr_to_int(tmp1, &address);
                address += CSADDR;
                chptr += 6;

                // length of the address field to be modified in halfbytes
                get_Nbytes(tmp2, chptr, 1);
                hexstr_to_int(tmp2, &halfbytenum);
                chptr += 2;

                deltaaddr = 0;
                if (chptr) {
                    strcpy(tmp1, chptr + 1);
                    if (hexstr_to_int(tmp1, &rfidx) == NO_ERR) {
                        deltaaddr = rftab[rfidx]; // reference number
                    } else {
                        es = get_es(tmp1);// symbol
                        deltaaddr = es->addr;
                    }
                    if (*chptr == '-') deltaaddr = -deltaaddr;
                }

                // execute relocation
                deltaaddr = CSADDR + deltaaddr;
                strcpy(tmp1, "");

                // concatenate 3 bytes from starting address
                sprintf(tmp2, "%02X", (int)MEM[address]);
                strcat(tmp1, tmp2);

                sprintf(tmp2, "%02X", (int)MEM[address+1]);
                strcat(tmp1, tmp2);

                sprintf(tmp2, "%02X", (int)MEM[address+2]);
                strcat(tmp1, tmp2);


                signed_6digit_hexstr_to_int(tmp1, &org_addr);
                new_addr = org_addr+deltaaddr;

                // update new address for symbols
                sprintf(tmp1, "%06X", new_addr);

                get_Nbytes(tmp2, tmp1, 1);
                hexstr_to_int(tmp2, &byteval);
                MEM[address] = (char)byteval;

                get_Nbytes(tmp2, tmp1 + 2, 1);
                hexstr_to_int(tmp2, &byteval);
                MEM[address + 2] = (char)byteval;

                get_Nbytes(tmp2, tmp1 + 4, 1);
                hexstr_to_int(tmp2, &byteval);
                MEM[address + 4] = (char)byteval;
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

    if (now == NULL) {
        ESTAB[index] = now;
        return;
    }
    while (now->next) {
        if (strcmp(now->name, es_name) == 0) {
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

void get_Nbytes(char *dest, char *src, int N) {
    strncpy(dest, src, N * 2);
    dest[N * 2] = '\0';
}