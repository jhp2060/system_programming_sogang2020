#include "assembler_commands.h"

error assemble(char* filename, int token_count) {
    if (token_count != 2) return ERR_WRONG_TOKENS;
   
    char program_name[MAX_LINE_LEN]; 
    FILE* fp = fopen(filename, "r");
    int len = strlen(filename), program_length;
    DIR* dir = opendir(filename);
    error e;
    
    if (!fp) return ERR_NOT_A_FILE;
    if (len < 5 || strcmp(filename + len - 4, ".asm") || dir)
	return ERR_NOT_A_ASM_FILE; 
    
    e = pass1(fp, &program_length, program_name);
    if (e != NO_ERR) {
	free_symtab(SYMTAB);
	return e; 
    }
    fclose(fp);
 
    e = pass2(program_name);
    if (e != NO_ERR) {
	free_symtab(SYMTAB);
	return e; 
    }
    
    // assembled successfully
    free_symtab(RECENT_SYMTAB);
    RECENT_SYMTAB = SYMTAB;
    printf("Successfully assembled %s.\n", filename);
    return NO_ERR;
}

// pass1 of assembler
error pass1(FILE* fp, int* program_length, char* program_name) {
    if (!fp) return ERR_NOT_A_FILE;
    char line[MAX_LINE_LEN];
    char label[MAX_LABEL_LEN], opcode[MAX_OPCODE_LEN];
    char op1[MAX_OPERAND_LEN], op2[MAX_OPERAND_LEN]; // sic/xe instructions have their operands at most 2
    char tmp_opcode[MAX_OPCODE_LEN];
    int locctr, starting_address = 0;
    char* ifn = ITM_FILE;
    FILE* ifp = fopen(ifn, "w");
    linetype lt;
    opcode_node* op_node;
    
    // read first input line
    read_line(fp, line);
    lt = parse(line, label, opcode, op1, op2);
    if (lt == LT_START) {
	starting_address = atoi(opcode);
	locctr = starting_address;
	strcpy(program_name, label);
	fprintf(ifp, "%04X %-10s %-10s %s %s\n", locctr, label, opcode, op1, op2);
	
	// read next line
	read_line(fp, line);
	lt = parse(line, label, opcode, op1, op2);
    }
    else return delete_file(ifp, ifn, ERR_NO_START);

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
	    else strcpy(tmp_opcode, opcode);
    	    if (lt == LT_OPCODE && (op_node = get_opcode(tmp_opcode)) != NULL) {
		if (strcmp(op_node->format, "3/4") == 0) {
		    if (opcode[0] == '+') locctr += 4; 				// format4
		    else locctr += 3;						// format3
		}
		else if (strcmp(op_node->format, "1") == 0) locctr += 1; 	// format1
		else if (strcmp(op_node->format, "2") == 0) locctr += 2;	// format2
	    }
	    else if (lt == LT_BASE);
	    else if (lt == LT_WORD) locctr += 3;
	    else if (lt == LT_RESW) locctr += (3 * atoi(op1));
	    else if (lt == LT_RESB) locctr += atoi(op1);
	    else if (lt == LT_BYTE) locctr += get_byte_length(op1);
	    else {
		printf("WRONG MNEMONIC: %s\n", opcode); 
		return delete_file(ifp, ifn, ERR_WRONG_MNEMONIC);	
	    }
	}
	else fprintf(ifp, ". %s\n", opcode);  
	read_line(fp, line);
	lt = parse(line, label, opcode, op1, op2);
    }
    fprintf(ifp, "%04X %-10s %-10s %s %s\n", locctr, label, opcode, op1, op2);
    fclose(ifp);
    *program_length = (locctr - starting_address);
    return NO_ERR;
}


error pass2(char* prefix) {
    FILE *fp = fopen(ITM_FILE, "r");
    FILE *lstfp, *objfp; 
    char filename[MAX_LINE_LEN];
    strcpy(filename, prefix);
    lstfp = fopen(strcat(filename, ".lst"), "w");
    strcpy(filename, prefix);
    objfp = fopen(strcat(filename, ".obj"), "w");
    fclose(lstfp);
    fclose(objfp);

	
     
    //return delete_file(fp, ITM_FILE, NO_ERR);
    fclose(fp);
    return NO_ERR;
}


// read a line from a file pointer, seperated by '\0' or '\n'
void read_line(FILE* fp, char* line) {
    fgets(line, MAX_LINE_LEN, fp);
    line[strlen(line) - 1] = '\0';
}

// tokenize a line, seperated into label, opcode, op1, op2
linetype parse(char* line, char* label, char* opcode, char* op1, char* op2) {
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
    if (buf[0] == '\0') return LT_NOT_A_LINE;
    if (isalpha(buf[0]) || buf[0] == '.') {
	strcpy(label, chptr);			// get label			
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
 
    strcpy(opcode, chptr);			// get opcode

    // get linetype
    if (strcmp(opcode, "START") == 0) ret = LT_START;
    else if (strcmp(opcode, "END") == 0) ret = LT_END;
    else if (strcmp(opcode, "BASE") == 0) ret = LT_BASE;
    else if (strcmp(opcode, "WORD") == 0) ret = LT_WORD;
    else if (strcmp(opcode, "RESW") == 0) ret = LT_RESW;
    else if (strcmp(opcode, "RESB") == 0) ret = LT_RESB;
    else if (strcmp(opcode, "BYTE") == 0) ret = LT_BYTE;
    else ret = LT_OPCODE;

    chptr = strtok(NULL, " \t");
    if (!chptr) return ret;
    strcpy(op1, chptr);				// get op1
    
    chptr = strtok(NULL, " \t");	
    if (!chptr) return ret;
    strcpy(op2, chptr);				// get op2
   
    return ret;
}

// initialize symtab as null, starting shell program
void init_symtab(void) {
    SYMTAB = NULL;
    RECENT_SYMTAB = NULL;
}

// free the allocated memories
void free_symtab(sym_node* head) {
    sym_node* now = head;
    sym_node* tmp = NULL;
    while (now) {
	tmp = now;
	now = now->next;
	free(tmp);
    }
    head = NULL;
}

// return whether that symbol exists in SYMTAB
int exists_in_symtab(char* symbol) {
    sym_node* now = SYMTAB;
    while (now) {
	if (strcmp(symbol, now->symbol) == 0) return 1;	
	now = now->next;
    }
   return 0;
}

// allocate new node for symbol table in the order of symbol
void push_symtab(char* symbol, int address) {
    sym_node* prev = NULL;
    sym_node* now = SYMTAB;
    sym_node* to_push = malloc(sizeof(sym_node));
    strcpy(to_push->symbol, symbol);
    to_push->address = address;
    to_push->next = NULL;
    if (now == NULL) {
	SYMTAB = to_push;
	return;
    }
    while (now->next) {
	if (strcmp(now->symbol, to_push->symbol) >= 0) { // alphabetical order
	    if (!prev) SYMTAB = to_push;
	    else prev->next = to_push;
	    to_push->next = now;
	    return;
	}
	prev = now;
	now = now->next;
    }
    now->next = to_push;
    to_push = NULL;
}

// calculate the byte constant's length
int get_byte_length(char* constant) {
    int ret;
    char* chptr;
    switch(constant[0]) {
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

// close the file which was on the writing, and delete it
error delete_file(FILE *fp, char* filename, error e) {
    fclose(fp);
    remove(filename);
    return e;
}
