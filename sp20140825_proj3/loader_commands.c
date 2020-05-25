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

    // initialize ESTAB
    free_estab();

    TOTLTH = 0;
    CSADDR = PROGADDR;
    EXECADDR = PROGADDR;

    // print loadmap
    printf("%-7s  %-7s  %-7s  %-7s\n", "control", "symbol", "address", "length");
    printf("%-7s  %-7s\n", "section", "name");
    printf("-----------------------------------\n");
    while (!is_nullstr(objfile)) {
        objfp = fopen(objfile, "r");
        if (!objfp) return ERR_NOT_A_FILE;

        loader_pass1(objfp);

        fclose(objfp);
        objfile = filenames[i++];
    }
    printf("-----------------------------------\n");
    printf("%-7s  %-7s  %-7s  %04X   \n", " ", "  total", "length", TOTLTH);

    i = 1;
    objfile = filenames[i++];

    CSADDR = PROGADDR;
    EXECADDR = PROGADDR;

    while (!is_nullstr(objfile)) {
        objfp = fopen(objfile, "r");
        if (!objfp) return ERR_NOT_A_FILE;

        loader_pass2(objfp);

        fclose(objfp);
        objfile = filenames[i++];
    }

    init_registers(TOTLTH, PROGADDR);
    bp_visited = 0;

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

    int limit = PROGADDR + CSLTH;
    error e;

    while (rgstr[R_PC] < limit) {
        if (BPCHK[rgstr[R_PC]] && !bp_visited) {
            dumpreg();
            printf("\t\tStop at checkpoint[%X]\n", rgstr[R_PC]);
            bp_visited = 1;
            return NO_ERR;
        }
        e = process_inst();
        if (e != NO_ERR) return e;
        bp_visited = 0;
    }
    dumpreg();
    printf("\t\tEnd Program\n");
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

    while (!feof(fp)) {
        read_line2(fp, line);
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
                printf("%-7s  %-7s  %04X     %04X   \n", tmp1, " ", CSADDR + address, CSLTH);

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

                    printf("%-7s  %-7s  %04X   \n", " ", tmp1, CSADDR + address);

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
    int startloc, halfbytenum, deltaaddr;
    es_node *es;
    int org_val, newval;

    while (!feof(fp)) {
        read_line2(fp, line);
        switch (line[0]) {
            case 'H':     // H record
                // get control section name
                chptr = strtok(line, " ");
                strcpy(tmp1, chptr + 1);

                es_node *csec = get_es(tmp1);
                rftab[1] = csec->addr;
                CSADDR = rftab[1];
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
                        // TODO error handling : no external symbol in estab
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
                hexstr_to_int(tmp1, &rgstr[R_PC]); // set PC
                rgstr[R_PC] += CSADDR;
                chptr += 6;

                // get length of object code
                get_Nbytes(tmp1, chptr, 1);
                hexstr_to_int(tmp1, &treclim);
                treclim += rgstr[R_PC];
                chptr += 2;

                // load byte by byte on the MEM
                while (rgstr[R_PC] <= treclim) {
                    get_Nbytes(tmp1, chptr, 1);
                    chptr += 2;
                    hexstr_to_int(tmp1, &byteval);
                    if (validate_value(byteval) != NO_ERR) {
                        printf("ERROR: wrong value to store!!!\n");
                        return;
                    }
                    MEM[rgstr[R_PC]++] = byteval;
                }
                break;
            case 'M':   // M record
                // get starting location of the address field to be modified
                chptr = line + 1;
                get_Nbytes(tmp1, chptr, 3);
                hexstr_to_int(tmp1, &startloc);
                startloc += CSADDR;
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

                // get value to modify
                // concatenate 3 bytes from starting address
                strcpy(tmp1, "");

                sprintf(tmp2, "%02X", MEM[startloc]);
                strcat(tmp1, tmp2);

                sprintf(tmp2, "%02X", MEM[startloc + 1]);
                strcat(tmp1, tmp2);

                sprintf(tmp2, "%02X", MEM[startloc + 2]);
                strcat(tmp1, tmp2);


                if (halfbytenum == 5) {
                    hexstr_to_int(tmp1 + 1, &org_val);
                } else if (halfbytenum == 6) {
                    signed_6digit_hexstr_to_int(tmp1, &org_val);
                }
                newval = org_val + deltaaddr;

                // update new address for symbols
                if (halfbytenum == 5) {
                    sprintf(tmp2, "%02X", MEM[startloc]);
                    sprintf(tmp1, "%c%05X", tmp2[0], newval);
                } else if (halfbytenum == 6) {
                    sprintf(tmp1, "%06X", newval);
                }

                // if newval is negative, tmp1 is 8 digit long
                if (strlen(tmp1) == 8) {
                    for (int i = 0; i <= 6; i++) tmp1[i] = tmp1[i + 2];
                }

                get_Nbytes(tmp2, tmp1, 1);
                hexstr_to_int(tmp2, &byteval);
                MEM[startloc] = byteval;

                get_Nbytes(tmp2, tmp1 + 2, 1);
                hexstr_to_int(tmp2, &byteval);
                MEM[startloc + 1] = byteval;

                get_Nbytes(tmp2, tmp1 + 4, 1);
                hexstr_to_int(tmp2, &byteval);
                MEM[startloc + 2] = byteval;
                break;
            case 'E':
                if (line[1] != '\0') {
                    get_Nbytes(tmp1, line + 1, 3);
                    hexstr_to_int(tmp1, &address);
                    EXECADDR = CSADDR + address;
                }
                break;
            default:
                break;
        }
    }

}

// print out the values of rgstr
void dumpreg(void) {
    printf("A : %06X   X : %06X\n", rgstr[R_A], rgstr[R_X]);
    printf("L : %06X  PC : %06X\n", rgstr[R_L], rgstr[R_PC]);
    printf("B : %06X   S : %06X\n", rgstr[R_B], rgstr[R_S]);
    printf("T : %06X\n", rgstr[R_T]);
}

error process_inst(void) {
    int opcode, format;
    int r1, r2;
    int flag_ni = 0, flag_x = 0, flag_b = 0, flag_p = 0, flag_e = 0;
    int disp = 0;

    int tgt_addr = 0, startloc = rgstr[R_PC], argument = 0;
    int tmp;

    opcode_node *op;

    da_flag = 0;

    // get opcode bytes at first
    opcode = MEM[startloc];
    flag_ni = opcode % 4;
    opcode -= flag_ni;
    op = get_opcode2(opcode);
    if (!op) return ERR_WRONG_OPCODE;

    // get format
    if (*(op->format) == '1') format =1;
    else if (*(op->format) == '2') format = 2;
    else {
        format = 3;
        flag_e = (MEM[startloc + 1] & 0b10000) >> 4;
        if (flag_e) format = 4;
    }

    rgstr[R_PC] += format;

    // get arguments from the instruction
    switch (format) {
        case 2:
            r1 = (MEM[startloc + 1] & 0b11110000) >> 4;
            r2 = MEM[startloc + 1] & 0b00001111;
            break;
        case 3:
        case 4:
            flag_x = (MEM[startloc+1] & 0b10000000) >> 7;
            flag_b =  (MEM[startloc+1] & 0b1000000) >> 6;
            flag_p =   (MEM[startloc+1] & 0b100000) >> 5;
            if (format == 3){
                disp = (MEM[startloc+1] & 0x0F) << 8;
                disp += MEM[startloc+2];
            }
            else {
                disp = (MEM[startloc+1] & 0x0F) << 16;
                disp += MEM[startloc+2] << 8;
                disp += MEM[startloc+3];
            }
            break;
        case 1:
        default:
            break;
    }

    // sign extension for negative disp value
    if(format == 3 && (disp & 0x800)) disp = disp | 0xFFFFF000;

    // get target address and argument value
    if (format >= 3){
        if (flag_b && !flag_p)      // b, p = 1, 0 : base relative addressing
            tgt_addr = rgstr[R_B] + disp;
        else if (!flag_b && flag_p) // b, p = 0, 1 : pc relative addressing
            tgt_addr = rgstr[R_PC] + disp;
        else {
            tgt_addr = disp;       // b, p = 0, 0 : direct addressing
            // da_flag = 1;
        }

        if (flag_x) tgt_addr += rgstr[R_X];
    }

    // execute opcode
    switch (opcode) {
        case 0xB4: //CLEAR
            rgstr[r1] = 0;
            break;
        case 0x28: //COMP
            argument = get_value(flag_ni, tgt_addr, 3, format);
            setCC(rgstr[R_A], argument);
            break;
        case 0xA0: //COMPR
            setCC(rgstr[r1], rgstr[r2]);
            break;
        case 0x3C: //J
            argument = get_addr(flag_ni, tgt_addr, format);
            rgstr[R_PC] = argument;
            break;
        case 0x30: //JEQ
            argument = get_addr(flag_ni, tgt_addr, format);
            if (CC == '=') rgstr[R_PC] = argument;
            break;
        case 0x38: //JLT
            argument = get_addr(flag_ni, tgt_addr, format);
            if (CC == '<') rgstr[R_PC] = argument;
            break;
        case 0x48: //JSUB
            argument = get_addr(flag_ni, tgt_addr, format);
            rgstr[R_L] = rgstr[R_PC];
            rgstr[R_PC] = argument;
            break;
        case 0x00: //LDA
            argument = get_value(flag_ni, tgt_addr, 3, format);
            rgstr[R_A] = argument;
            break;
        case 0x68: //LDB
            argument = get_value(flag_ni, tgt_addr, 3, format);
            rgstr[R_B] = argument;
            break;
        case 0x50: //LDCH
            argument = get_value(flag_ni, tgt_addr, 1, format);
            tmp = 0x000000FF & argument;     // rightmost byte
            rgstr[R_A] = rgstr[R_A] & 0xFFFFFF00;
            rgstr[R_A] = rgstr[R_A] | tmp;
            break;
        case 0x70: //LDF
            argument = get_value(flag_ni, tgt_addr, 3, format);
            rgstr[R_F] = argument;
            break;
        case 0x08: //LDL
            argument = get_value(flag_ni, tgt_addr, 3, format);
            rgstr[R_L] = argument;
            break;
        case 0x6C: //LDS
            argument = get_value(flag_ni, tgt_addr, 3, format);
            rgstr[R_S] = argument;
            break;
        case 0x74: //LDT
            argument = get_value(flag_ni, tgt_addr, 3, format);
            rgstr[R_T] = argument;
            break;
        case 0x04: //LDX
            argument = get_value(flag_ni, tgt_addr, 3, format);
            rgstr[R_X] = argument;
            break;
        case 0xD8: //RD
            rgstr[R_A] = rgstr[R_A] & 0xFFFFFF00;
            break;
        case 0x4C: //RSUB
            rgstr[R_PC] = rgstr[R_L];
            break;
        case 0x0C: //STA
            store_3bytes(flag_ni, tgt_addr, rgstr[R_A], format);
            break;
        case 0x78: //STB
            store_3bytes(flag_ni, tgt_addr, rgstr[R_B], format);
            break;
        case 0x54: //STCH
            store_1byte(flag_ni, tgt_addr, rgstr[R_A], format);
            break;
        case 0x80: //STF
            store_3bytes(flag_ni, tgt_addr, rgstr[R_F], format);
            break;
        case 0x14: //STL
            store_3bytes(flag_ni, tgt_addr, rgstr[R_L], format);
            break;
        case 0x7C: //STS
            store_3bytes(flag_ni, tgt_addr, rgstr[R_S], format);
            break;
        case 0xE8: //STSW
            store_3bytes(flag_ni, tgt_addr, rgstr[R_SW], format);
            break;
        case 0x84: //STT
            store_3bytes(flag_ni, tgt_addr, rgstr[R_T], format);
            break;
        case 0x10: //STX
            store_3bytes(flag_ni, tgt_addr, rgstr[R_X], format);
            break;
        case 0xE0: //TD
            CC = '<';
            break;
        case 0xB8: //TIXR
            rgstr[R_X] += 1;
            setCC(rgstr[R_X], rgstr[r1]);
            break;
        case 0xDC: //WD
            break;
        default:
            printf("WRONG ACCESS at 0x%02X\n", rgstr[R_PC]);
            return ERR_WRONG_OPCODE;
            break;
    }

    return NO_ERR;
}

// compare two arguments and set CC
void setCC(int val1, int val2){
    if (val1 > val2) CC = '>';
    else if (val1 < val2) CC = '<';
    else CC ='=';
}

// get value from MEM with a certain byte size
int get_value(int flag_ni, int startloc, int bytenum, int format) {
    int ret = 0, i = 0;

    if (format == 4 || da_flag) return startloc;

    switch(flag_ni){
        case 1:     // n, i = 0, 1 : immediate_ addressing
            ret = startloc;
            break;
        case 2:     // n, i = 1, 0 : indirect_addressing
            startloc = MEM[startloc];
            for (i = 0; i < bytenum; i++)
                ret += (MEM[startloc + bytenum - i - 1] << (i * 8));
            break;
        case 3:     // n, i = 1, 1 : simple addressing
            for (i = 0; i < bytenum; i++)
                ret += (MEM[startloc + bytenum - i - 1] << (i * 8));
            break;
        case 0:     // n, i = 0, 0 : don't use disp
            ret = -1;
        default:
            break;
    }
    return ret;
}

// get address to jump into
int get_addr(int flag_ni, int tgt_addr, int format) {
    int ret = 0;

    if (format == 4) return tgt_addr;

    switch(flag_ni){
        case 2: // n, i = 1, 0 : indirect addressing
            ret += MEM[tgt_addr + 2];
            ret += (MEM[tgt_addr + 1] << 8);
            ret += (MEM[tgt_addr] << 16);
            break;
        case 3:
            ret = tgt_addr;
            break;
        case 0: // no address impossible
        case 1: // immediate addressing impossible.
        default:
            break;
    }
    return ret;
}

// store a value into 3 MEMs
void store_3bytes(int flag_ni, int startloc, int to_store, int format) {

    switch(flag_ni){
        case 2:     // n, i = 1, 0 : indirect_addressing
            if (da_flag) break;
            startloc = MEM[startloc];
            break;
        case 3:     // n, i = 1, 1 : simple addressing
            startloc = startloc;
            break;
        case 0:     // n, i = 0, 0 : don't use disp
        case 1:     // n, i = 0, 1 : immediate_ addressing. impossible.
        default:
            break;
    }

    if (format == 4) startloc = startloc;

    MEM[startloc + 2] =  to_store & 0x000000FF;
    MEM[startloc + 1] = (to_store & 0x0000FF00) >> 8;
    MEM[startloc]     = (to_store & 0x00FF0000) >> 16;
}

// store a value into a MEM
void store_1byte(int flag_ni, int startloc, int to_store, int format) {
    switch(flag_ni){
        case 2:     // n, i = 1, 0 : indirect_addressing
            if(da_flag) break;
            startloc = MEM[startloc];
            break;
        case 3:     // n, i = 1, 1 : simple addressing
            startloc = startloc;
            break;
        case 0:     // n, i = 0, 0 : don't use disp
        case 1:     // n, i = 0, 1 : immediate_ addressing. impossible.
        default:
            break;
    }

    if (format == 4) startloc = startloc;

    MEM[startloc] =  to_store & 0x000000FF;
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
        ESTAB[index] = to_push;
        return;
    }
    while (now->next) {
        if (strcmp(now->name, es_name) == 0) {
            // TODO external symbol 중복
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
            free(tmp);
        }
        ESTAB[i] = NULL;
    }
}


// initialize the values of rgstr
void init_registers(int l, int pc) {
    int i = 0;
    for (i = 0; i <= R_SW; i++)
        rgstr[i] = 0;
    rgstr[R_L] = l;
    rgstr[R_PC] = pc;
}

void get_Nbytes(char *dest, char *src, int N) {
    strncpy(dest, src, N * 2);
    dest[N * 2] = '\0';
}

void read_line2(FILE *fp, char *line) {
    fgets(line, MAX_LINE_LEN, fp);
    if (line[strlen(line) - 1] == '\n') {
        line[strlen(line) - 1] = '\0';
    }
}

