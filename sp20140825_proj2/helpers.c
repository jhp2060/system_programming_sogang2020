#include "helpers.h"

// return the boolean value expressing whether the norm is same with either one of the operands
int strcmp_twice(char* norm, char* op1, char* op2) {
    return (strcmp(norm, op1) == 0 || strcmp(norm, op2) == 0);
}

// transform the hexa string into int vlaue
error hexstr_to_int(char* str, int *ret) {
    int i, now, mult = 1;
	*ret = 0;
    int len = strlen(str);
    for (i = len - 1; i >= 0; i--) {
        if (str[i] >= '0' && str[i] <= '9') now = str[i] - '0';
        else if (str[i] >= 'A' && str[i] <= 'F')	// for the uppercase
            now = str[i] - 'A' + 10;
	else if (str[i] >= 'a' && str[i] <= 'f')	// for the lowercase
	    now = str[i] - 'a' + 10;
        else return ERR_WRONG_HEXSTR;			// wrong format of hexa string
        *ret += (now * mult);
        mult *= 16;
    }
	return NO_ERR;
}

// print out the error messages according to the error
void print_error_msg(error e){
    switch(e) {
    case NO_ERR:
    case NO_ERR_HISTORY:
		break;
    case ERR_WRONG_COMMAND:
		printf("ERROR: wrong command to execute.\n");
		break;
    case ERR_WRONG_HEXSTR:
		printf("ERROR: wrong hexadecimal string to turn into int.\n");
		break;
    case ERR_ARGS_FORMAT:
		printf("ERROR: should use ',' between two arguments.\n");
		break;
    case ERR_WRONG_ADDR:
		printf("ERROR: wrong address to access.\n");
		break;
    case ERR_WRONG_VAL:
		printf("ERROR: wrong value to store.\n");
		break;
    case ERR_WRONG_RANGE:
		printf("ERROR: wrong range.\n");
		break;
    case ERR_WRONG_TOKENS:
		printf("ERROR: wrong argument numbers.\n");
		break;
    case ERR_WRONG_MNEMONIC:
		printf("ERROR: no opcode for that mnemonic.\n"); 
		break;
    case ERR_NOT_A_FILE:
		printf("ERROR: no file to open.\n");
		break;
    case ERR_NOT_A_ASM_FILE:
		printf("ERROR: not an .asm file.\n");
		break;
    case ERR_NO_END:
		printf("ERROR: no END mnemonic in this .asm file.\n");
		break;
    case ERR_SYMBOL_DUPLICATED:
		printf("ERROR: duplicated symbol does exist.\n");
		break;
    case ERR_NO_INTERMEDIATE_FILE:
		printf("ERROR: no intermediate file to load in pass2.\n");
		break;
    case ERR_NO_INST_OPERAND:
		printf("ERROR: 1 or more operands needed for the instruction.\n");
		break;
    case ERR_NO_SYMBOL:
		printf("ERROR: no symbol with that name in SYMTAB.\n");
		break;
    case ERR_NO_INST_COMMA:
		printf("ERROR: a comma(,) needed between two operands in the instruction.\n");
		break;
    default:
		break;
    }
}

// whether the string is null string or not
int is_nullstr(char* str) {
    return (*(str) == '\0');
}

// convert an int value into binary string
void int_to_binstr (char* ret, int val, int digit) {
    if (digit > 16) return;
    int i, mask;
    for (i = digit - 1; i >= 0; i--) {
        mask = 1 << i;
        ret[i] = mask & val ? '1' : '0';
    }
    ret[digit] = '\0';
}
