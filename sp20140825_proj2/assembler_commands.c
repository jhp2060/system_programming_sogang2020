#include "assembler_commands.h"

error assemble(char *filename, int token_count) {
    if (token_count != 2) return ERR_WRONG_TOKENS;

    FILE *fp = fopen(filename, "r");
    int len = strlen(filename), program_length;
    DIR *dir = opendir(filename);
    char *prefix, *suffix, tmp[MAX_LINE_LEN];
    error e;

    if (!fp || dir) return ERR_NOT_A_FILE;

    //get prefix and suffix
    strcpy(tmp, filename);
    prefix = strtok(tmp, ".");
    if (!prefix) {
        fclose(fp);
        return ERR_NOT_A_ASM_FILE;
    }
    suffix = strtok(NULL, "");
    if (len < 5 || strcmp(suffix, "asm") != 0) {
        fclose(fp);
        return ERR_NOT_A_ASM_FILE;
    }

    // initialize base_register
    base_register = -1;
    init_symtab();

    e = pass1(fp, prefix, &program_length);
    fclose(fp);
    if (e != NO_ERR) {
        free_symtab(SYMTAB);
        return e;
    }

    e = pass2(prefix, program_length);
    if (e != NO_ERR) {
        free_symtab(SYMTAB);
        return e;
    }

    // assembled successfully
    free_symtab(RECENT_SYMTAB);
    RECENT_SYMTAB = SYMTAB;        // store recent symtab
    SYMTAB = NULL;
    preload_registers_on_symtab();
    printf("Successfully assembled %s.\n", filename);
    return NO_ERR;
}

// execute command symbol : print out the symtab
error symbol(int token_count) {
	if (token_count != 1) return ERR_WRONG_TOKENS;
	sym_node* now = RECENT_SYMTAB;
	while (now) {
		char s0 = now->symbol[0];
		char s1 = now->symbol[1];
		switch (s0) {
		case 'A':
		case 'X':
		case 'L':
		case 'B':
		case 'T':
		case 'F':
			if (s1 == '\0') break;
		case 'P':
			if (s1 == 'C') break;
		case 'S':
			if (s1 == '\0' || s1 == 'W') break;
		default:
			printf("\t%-10s %04X\n", now->symbol, now->address);
			break;
		}
		now = now->next;
	}
	return NO_ERR;
}

// pass1 of assembler
error pass1(FILE *fp, char *prefix, int *program_length) {
    if (!fp) return ERR_NOT_A_FILE;

    // variables for parsing
    char line[MAX_LINE_LEN];
    char label[MAX_LABEL_LEN], opcode[MAX_OPCODE_LEN];
    char op1[MAX_OPERAND_LEN], op2[MAX_OPERAND_LEN];    // sic/xe instructions have their operands at most 2
    char tmp_opcode[MAX_OPCODE_LEN];

    // variables for locctr
    int locctr, starting_address = 0;
    int locctr_delta = 0;
    linetype lt;
    opcode_node *op_node;

    // variales for files
    char ifn[MAX_FILENAME_LEN];
    FILE *ifp;

    strcpy(ifn, prefix);
    strcat(ifn, ".itm");
    ifp = fopen(ifn, "w");

    // read first input line
    read_line(fp, line);
    lt = parse(line, label, opcode, op1, op2);
    if (lt == LT_START) {
        starting_address = atoi(opcode);
        locctr = starting_address;
        fprintf(ifp, "%04X %-10s %-10s %s %s\n", locctr, label, opcode, op1, op2);

        // read next line
        read_line(fp, line);
        lt = parse(line, label, opcode, op1, op2);
    } else {
        starting_address = 0;
        locctr = 0;
    }

    // while opcode != END
    while (lt != LT_END) {
        if (feof(fp)) return delete_file(ifp, ifn, ERR_NO_END);
        if (lt != LT_COMMENT) {
            if (!is_nullstr(label)) {
                if (exists_in_symtab(label))
                    return delete_file(ifp, ifn, ERR_SYMBOL_DUPLICATED);
                push_symtab(label, locctr);
            }
            fprintf(ifp, "%04X %-10s %-10s %s %s\n", locctr, label, opcode, op1, op2);
            if (opcode[0] == '+') strcpy(tmp_opcode, opcode + 1);
            else
                strcpy(tmp_opcode, opcode);
            if (lt == LT_OPCODE && (op_node = get_opcode(tmp_opcode)) != NULL) {
                if (strcmp(op_node->format, "3/4") == 0) {
                    if (opcode[0] == '+') locctr_delta = 4;            // format4
                    else locctr_delta = 3;                    // format3
                } else if (strcmp(op_node->format, "1") == 0) locctr_delta = 1;    // format1
                else if (strcmp(op_node->format, "2") == 0) locctr_delta = 2;    // format2
            } else if (lt == LT_BASE || lt == LT_NOBASE) locctr_delta = 0;
            else if (lt == LT_WORD) locctr_delta = 3;
            else if (lt == LT_RESW) locctr_delta = (3 * atoi(op1));
            else if (lt == LT_RESB) locctr_delta = atoi(op1);
            else if (lt == LT_BYTE) locctr_delta = get_byte_length(op1);
            else {
                printf("WRONG MNEMONIC: %s\n", opcode);                // validate instruction
                return delete_file(ifp, ifn, ERR_WRONG_MNEMONIC);
            }
            locctr += locctr_delta;
        } else fprintf(ifp, "%04X %-10s %s\n", locctr, label, opcode);
        read_line(fp, line);
        lt = parse(line, label, opcode, op1, op2);
    }

    fprintf(ifp, "%04X %-10s %-10s %s %s\n", locctr, label, opcode, op1, op2);

    fclose(ifp);
    *program_length = (locctr - starting_address);
    return NO_ERR;
}

// pass 2 for sic/xe assembler
error pass2(char *prefix, int program_length) {
    char ifn[MAX_FILENAME_LEN];
    FILE *ifp;

    // get the intermediate file
    strcpy(ifn, prefix);
    strcat(ifn, ".itm");
    ifp = fopen(ifn, "r");
    if (!ifp) return ERR_NO_INTERMEDIATE_FILE;

    // variables for storing parsing results
    char line[MAX_LINE_LEN];
    char label[MAX_LABEL_LEN], opcode[MAX_OPCODE_LEN];
    char op1[MAX_OPERAND_LEN], op2[MAX_OPERAND_LEN]; // sic/xe instructions have their operands at most 2

    // variables for .lst, .obj files
    FILE *lstfp, *objfp;
    char filename[MAX_LINE_LEN], program_name[MAX_LINE_LEN];
    char tmp_opcode[MAX_LINE_LEN];
    char object_code[MAX_OBJECT_CODE_LEN];
    char now_text_record[1 + 6 + 2 + MAX_TEXT_RECORD_LEN * 20 + 1]; // T addr(6) length(2) textrecord \0(1)
    int locctr = 0, linenum = 1, starting_address = 0;
    int now_text_record_len = 0, sepflag = 0;
    linetype lt, prev_lt;
    opcode_node *op_node;
    sym_node *s_node;
    error e;

	init_modi_records();	

    // create .lst, .obj files
    strcpy(filename, prefix);
    lstfp = fopen(strcat(filename, ".lst"), "w");
    strcpy(filename, prefix);
    objfp = fopen(strcat(filename, ".obj"), "w");

    // read first input line
    read_line(ifp, line);
    lt = parse2(line, &locctr, label, opcode, op1, op2);
    if (lt == LT_START) {
        // write listing file
        fprintf(lstfp, "%3d %-40s\n", (linenum++) * LINE_MULTIPLIER, line);
        starting_address = locctr;
        strcpy(program_name, label);

        // read next line
        read_line(ifp, line);
        lt = parse2(line, &locctr, label, opcode, op1, op2);
    } else {
        locctr = 0;
        starting_address = 0;
    }

    fprintf(objfp, "H%-6s%06X%06X\n", program_name, starting_address, program_length);    // header record
    sprintf(now_text_record, "T%06X", starting_address);

    // while opcode != END
    while (lt != LT_END) {
        if (feof(ifp)) return assemble_failed(lstfp, objfp, prefix, ERR_NO_END);
		strcpy(object_code, "\0");
        if (lt != LT_COMMENT) {
			// get '+' removed from opcode
            if (opcode[0] == '+') strcpy(tmp_opcode, opcode + 1);
            else strcpy(tmp_opcode, opcode);
		
			// if found opcode in optab
            if (lt == LT_OPCODE && (op_node = get_opcode(tmp_opcode)) != NULL) {
                e = get_object_code(object_code, locctr, opcode, op1, op2);
                if (e != NO_ERR) return assemble_failed(lstfp, objfp, prefix, e);
            } 
			else if (lt == LT_BYTE) { // convert constant to object code
				char tmp_op1[MAX_OPERAND_LEN];
                if (op1[0] == 'C') {
					strcpy(tmp_op1, op1);
                    char tmpbuf[3];
                    char *chptr = strtok(tmp_op1, " C'`");
                    for (int i = 0; i < (int) strlen(chptr); i++) {
                        sprintf(tmpbuf, "%02X", (int) *(chptr + i));
                        strcat(object_code, tmpbuf);
                    }
                } 
				else if (op1[0] == 'X') {
					strcpy(tmp_op1, op1);
					char *chptr = strtok(tmp_op1, " X'`");
					strcpy(object_code, chptr);
                }
            } 
			else if (lt == LT_WORD) sprintf(object_code, "%06X", atoi(op1)); 	// convert constant to object code
			else if (lt == LT_BASE) { 		// set base
                s_node = get_symbol(op1);
                if (!s_node) {
                    printf("WRONG SYMBOL: %s\n", op1);
                    return assemble_failed(lstfp, objfp, prefix, ERR_NO_SYMBOL);
                }
                base_register = s_node->address;
				locctr = -1;
            } 
			else if (lt == LT_NOBASE) {
                base_register = -1;
				locctr = -1;
            }
			
			// if object code will not fit into the currnet text record
			if ((lt == LT_RESW || lt == LT_RESB) && (prev_lt != LT_RESW && prev_lt != LT_RESB)) sepflag = 1; 
			if (now_text_record_len + strlen(object_code) / 2 > MAX_TEXT_RECORD_LEN) sepflag = 1;
			if (sepflag) {
				sepflag = 0;
                update_text_record_len(now_text_record, now_text_record_len);
                fprintf(objfp, "%s\n", now_text_record);                // write text record on obj file
				now_text_record_len = 0;
                sprintf(now_text_record, "T%06X", locctr);				// initialize new text record
            }
			
			// add object code to text record
			now_text_record_len += strlen(object_code) / 2;				// length metric is a byte, which is presented as 2 hexadecimal number(2 characters)
			strcat(now_text_record, object_code);
        }
		else {
			locctr = -1;
			strcpy(object_code, "\0");
		}

		// write listing line 
		if (locctr >= 0) fprintf(lstfp, "%3d %-40s %-s\n", (linenum++) * LINE_MULTIPLIER, line, object_code);
		else fprintf(lstfp, "%3d %-4s %-35s %-s\n", (linenum++) * LINE_MULTIPLIER, "", line + 5, object_code);

		// read next input line
        read_line(ifp, line);
		prev_lt = lt;
        lt = parse2(line, &locctr, label, opcode, op1, op2);
    }

    // write last text record to object program
    update_text_record_len(now_text_record, now_text_record_len);
    fprintf(objfp, "%s\n", now_text_record);

    // write modification records to object program
	for (int i = 0; i < modi_record_num; i++) {
		fprintf(objfp, "%s\n", modi_records[i]);
	}

    // write end record to object program
    fprintf(objfp, "E%06X", starting_address);

    // write last listing line 
	fprintf(lstfp, "%3d %-4s %-35s", (linenum++) * LINE_MULTIPLIER, "", line + 5);

    fclose(lstfp);
    fclose(objfp);
	
	// delete intermediate file
    return delete_file(ifp, ifn, NO_ERR);
}


// read a line from a file pointer, seperated by '\0' or '\n'
void read_line(FILE *fp, char *line) {
    fgets(line, MAX_LINE_LEN, fp);
    line[strlen(line) - 1] = '\0';
}

// tokenize a line, seperated into label, opcode, op1, op2
linetype parse(char *line, char *label, char *opcode, char *op1, char *op2) {
    char buf[MAX_LINE_LEN], *chptr;
    linetype ret;
    strcpy(buf, line);

    // initialize value 
    strcpy(label, "\0");
    strcpy(opcode, "\0");
    strcpy(op1, "\0");
    strcpy(op2, "\0");

    // parsing (tokenize)
    chptr = strtok(buf, " \t");
    if (!chptr || is_nullstr(chptr)) return LT_NOT_A_LINE;
    if (isalpha(buf[0]) || buf[0] == '.') {
        strcpy(label, chptr);            // get label
        chptr = strtok(NULL, " \t");
        if (buf[0] == '.') {
            ret = LT_COMMENT;
            if (!chptr) return ret;
        }
    }

    if (ret == LT_COMMENT) {
        *(chptr + strlen(chptr)) = ' ';
        strcpy(opcode, chptr);
        return ret;
    }

    strcpy(opcode, chptr);            // get opcode

    // get linetype
    if (strcmp(opcode, "START") == 0) ret = LT_START;
    else if (strcmp(opcode, "END") == 0) ret = LT_END;
    else if (strcmp(opcode, "BASE") == 0) ret = LT_BASE;
    else if (strcmp(opcode, "NOBASE") == 0) ret = LT_NOBASE;
    else if (strcmp(opcode, "WORD") == 0) ret = LT_WORD;
    else if (strcmp(opcode, "RESW") == 0) ret = LT_RESW;
    else if (strcmp(opcode, "RESB") == 0) ret = LT_RESB;
    else if (strcmp(opcode, "BYTE") == 0) ret = LT_BYTE;
    else ret = LT_OPCODE;

    chptr = strtok(NULL, " \t");
    if (!chptr) return ret;
    strcpy(op1, chptr);                // get op1

    chptr = strtok(NULL, " \t");
    if (!chptr) return ret;
    strcpy(op2, chptr);                // get op2

    return ret;
}

// parsing for pass2
linetype parse2(char *line, int *locctr, char *label, char *opcode, char *op1, char *op2) {
    char buf[MAX_LINE_LEN], *chptr;
    strcpy(buf, line);

    chptr = strtok(buf, " \t");
    if (!chptr || is_nullstr(chptr)) return LT_NOT_A_LINE;
    hexstr_to_int(chptr, locctr);
    return parse(chptr + strlen(chptr) + 1, label, opcode, op1, op2);
}

// initialize symtab as null, starting shell program
void init_symtab(void) {
    SYMTAB = NULL;
    //RECENT_SYMTAB = NULL;
    preload_registers_on_symtab();
}

// free the allocated memories
void free_symtab(sym_node *head) {
    sym_node *now = head;
    sym_node *tmp = NULL;
    while (now) {
        tmp = now;
        now = now->next;
        free(tmp);
    }
    head = NULL;
}

// return whether that symbol exists in SYMTAB
int exists_in_symtab(char *symbol) {
    sym_node *now = SYMTAB;
    while (now) {
        if (strcmp(symbol, now->symbol) == 0) return 1;
        now = now->next;
    }
    return 0;
}

// allocate new node for symbol table in the order of symbol
void push_symtab(char *symbol, int address) {
    sym_node *prev = NULL;
    sym_node *now = SYMTAB;
    sym_node *to_push = malloc(sizeof(sym_node));
    strcpy(to_push->symbol, symbol);
    to_push->address = address;
    to_push->next = NULL;
    if (now == NULL) {
        SYMTAB = to_push;
        return;
    }
    while (now->next) {
        if (strcmp(now->symbol, to_push->symbol) > 0) { // alphabetical order
            if (!prev) SYMTAB = to_push;
            else prev->next = to_push;
            to_push->next = now;
            return;
        }
        // TODO error handling for symbol duplicated
        prev = now;
        now = now->next;
    }
    now->next = to_push;
    to_push = NULL;
}

// preload the registers on SYMTAB
void preload_registers_on_symtab(void) {
    push_symtab("A", 0);
    push_symtab("X", 1);
    push_symtab("L", 2);
    push_symtab("PC", 8);
    push_symtab("SW", 9);
    push_symtab("B", 3);
    push_symtab("S", 4);
    push_symtab("T", 5);
    push_symtab("F", 6);
}

// find the symbol and return the symbol_node
sym_node *get_symbol(char *symbol) {
    sym_node *now = SYMTAB;
    while (now != NULL) {
        if (strcmp(now->symbol, symbol) == 0) return now;
        now = now->next;
    }
    return now;
}

// calculate the byte constant's length
int get_byte_length(char *constant) {
    int ret;
    char *chptr;
    switch (constant[0]) {
        case 'C': // characters
            chptr = strtok(constant, "C`'");
            ret = strlen(chptr);
            break;
        case 'X': // a hexadecimal
            chptr = strtok(constant, "X`'");
            ret = strlen(chptr) / 2;
            break;
    }
    return ret;
}

// get object code with a line
error get_object_code(char *ret, int pc, char *opcode, char *op1, char *op2) {
    int format = -1;
    int n, i, x, b, p, e, addr = 0;
    char tmp_opcode[MAX_OPCODE_LEN], tmp_op1[MAX_OPCODE_LEN], modi_str[MODI_RECORD_LEN], *chptr;
    opcode_node *op_node;
    sym_node *sym1, *sym2;
    error err = NO_ERR;

    if (opcode[0] == '+') {
        strcpy(tmp_opcode, opcode + 1);
        e = 1;
    } else {
        strcpy(tmp_opcode, opcode);
        e = 0;
    }
    op_node = get_opcode(tmp_opcode);        // op_node validation already conducted in pass1

    if (e == 1) format = 4;
    else if (strcmp(op_node->format, "3/4") == 0) format = 3;
    else if (strcmp(op_node->format, "1") == 0) format = 1;
    else if (strcmp(op_node->format, "2") == 0) format = 2;
	
	pc += format;
	
    switch (format) {
        case 1:
            sprintf(ret, "%02X", op_node->opcode);
            break;
        case 2:
            // the operands should be in symtab (must be registers)
            if (is_nullstr(op1)) {
                printf("INSTRUCTION OPERAND NEEDED: %s\n", tmp_opcode);
                return ERR_NO_INST_OPERAND;
            }

            strcpy(tmp_op1, op1);
            if (op1[strlen(op1) - 1] == ',') tmp_op1[strlen(op1) - 1] = '\0';
            sym1 = get_symbol(tmp_op1);
            if (!sym1) {
                printf("WRONG SYMBOL: %s\n", tmp_op1);
                return ERR_NO_SYMBOL;
            }

            if (is_nullstr(op2)) { // only one operand
                sprintf(ret, "%02X%X%X", op_node->opcode, sym1->address, 0);
                break;
            } 
			else {  // 2 operands needed
                if (op1[strlen(op1) - 1] != ',') {
                    printf("NO COMMA BETWEEN: %s and  %s\n", op1, op2);
                    return ERR_NO_INST_COMMA;
                }
                strtok(op1, " ,");
                sym2 = get_symbol(op2);
                if (!sym2) {
                    printf("WRONG SYMBOL: %s\n", op2);
                    return ERR_NO_SYMBOL;
                }
                sprintf(ret, "%02X%X%X", op_node->opcode, sym1->address, sym2->address);
            }
            break;
		case 3:
        case 4:
            // n, i
            if (op1[0] == '#') n = 0, i = 1;        // immediate addressing
            else if (op1[0] == '@') n = 1, i = 0;        // indirect addressing
            else n = 1, i = 1;                // simple addressing

            // x
            if (op2[0] == 'X') x = 1;
            else x = 0;

            // b, p : pc relative first, if not available, base relative
            chptr = strtok(op1, " #@,");
			if (!chptr) {
				b = 0, p = 0;
				addr = 0;
			}
			else {
            	sym1 = get_symbol(chptr);
	            if (sym1) {
    	            if (strcmp(opcode, "LDB") == 0) base_register = sym1->address;										// set base
        	        if (format == 3) {
            	        if (-2048 <= sym1->address - pc && sym1->address - pc <= 2047) {							// pc-relative
                	        b = 0, p = 1;    
                    	    addr = sym1->address - pc;
                    	}
					 	else if (0 <= sym1->address - base_register && sym1->address - base_register <= 4095) {		// base-relative
                        	b = 1, p = 0;    	
                        	addr = sym1->address - base_register;
						}
						else return ERR_WRONG_ADDR;
					}
					else {    				// format4 => direct addressing with symbol
						b = 0, p = 0;
                    	addr = sym1->address;
						sprintf(modi_str, "M%06X%02X", pc - 3, 5);
						strcpy(modi_records[modi_record_num++], modi_str);						
                	}
				}
				else if (op1[0] == '#' && is_constant(chptr)) {
					addr = atoi(op1+1);
					b = 0, p = 0;
            	}
				else return ERR_NO_SYMBOL;
			}

            if (format == 3)
				sprintf(ret, "%02X%0X%03X", (op_node->opcode + 2 * n + i), (8 * x + 4 * b + 2 * p + e), addr & 0b111111111111);
    	    else if (format == 4)
				sprintf(ret, "%02X%0X%05X", (op_node->opcode + 2 * n + i), (8 * x + 4 * b + 2 * p + e), addr & 0b11111111111111111111);
			break;
    }
    return err;
}

// 
void update_text_record_len(char *text_record, int len) {
    char tmp[3];
    sprintf(tmp, "%02X", len);
    text_record[7] = tmp[0];
    text_record[8] = tmp[1];
}

int is_constant(char *symbol) {
    int i;
    for (i = 0; i < (int) strlen(symbol); i++) {
        if (!isdigit(symbol[i])) return 0;
    }
    return 1;
}

void init_modi_records(void) {
	int i;
	modi_record_num = 0;
	for (i = 0; i < MAX_MODI_RECORD; i++) {
		strcpy(modi_records[i], "\0");
	}
}


// close the file which was on the writing, and delete it
error delete_file(FILE *fp, char *filename, error e) {
    fclose(fp);
    remove(filename);
    return e;
}

// close the files on writing and delete them(.lst, .obj) in pass2
error assemble_failed(FILE *lstfp, FILE *objfp, char *prefix, error e) {
    char filename[MAX_FILENAME_LEN];
    strcpy(filename, prefix);

    fclose(lstfp);
    fclose(objfp);

    remove(strcat(filename, ".lst"));
    strcpy(filename, prefix);
    remove(strcat(filename, ".obj"));

    return e;

}
