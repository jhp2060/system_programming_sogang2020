#include "memory_commands.h"

// print out the memory between start and end
int dump(char* start, char* end, int token_count) {
    int row, col;
    int st, ed, err_code;
    
    // argument parsing and validation
    switch (token_count) {
	case 1: // no arguments
	    st = LAST_ADDR + 1 < MEM_SIZE ? LAST_ADDR + 1 : 0;
	    ed = st + 16 * 10 - 1 < MEM_SIZE ? st + 16 * 10 - 1 : MEM_SIZE - 1;
	    break;
	case 2: // 1 argument
	    err_code = validate_one_hexstr_argument(start, &st);
	    if (err_code != NO_ERR) return err_code;
	    ed = st + 16 * 10 - 1 < MEM_SIZE ? st + 16 * 10 - 1 : MEM_SIZE - 1;
	    break;	    
	case 3: // 2 arguments
	    err_code = validate_two_hexstr_arguments(start, end, &st, &ed);
	    if (err_code != NO_ERR) return err_code;
	    break;
	default:
	   return ERR_WRONG_TOKENS; 
    }
    err_code = validate_range(st, ed);
    if (err_code != NO_ERR) return err_code;

    // print out the memory values in the valid range
    int st_row = st / 16, ed_row = ed / 16;
    if (ed_row < 0) ed_row = 0;
    for (row = st_row; row <= ed_row; row++) {
	printf("%05X  ", row * 16); 
	for (col = 0; col < 16; col++) {
	    int now = row * 16 + col;
	    if (now < st || now > ed) printf("   ");
	    else printf("%02X ", MEM[now]);
	}
	printf(";  ");
	print_chars(row);
    }
    LAST_ADDR = ed;
    return NO_ERR;
}

// edit the value of memory at the address
int edit(char* address, char* value, int token_count) {
    if (token_count != 3) return ERR_WRONG_TOKENS;
    int addr, val;
    int err_code = validate_two_hexstr_arguments(address, value, &addr, &val);
    if (err_code != NO_ERR) return err_code;
    
    err_code = validate_address(addr);
    if (err_code != NO_ERR) return err_code;
    
    err_code = validate_value(val);
    if (err_code != NO_ERR) return err_code;

    MEM[addr] = val;
    return NO_ERR;
}

// fill the memory spaces between start and end with a value
int fill(char* start, char* end, char* value, int token_count) {
    if (token_count != 4) return ERR_WRONG_TOKENS;

    int st, ed, val, i;
    int err_code = validate_three_hexstr_arguments(start, end, value, &st, &ed, &val);

    err_code = validate_range(st, ed);
    if (err_code != NO_ERR) return err_code;

    err_code = validate_value(val);
    if (err_code != NO_ERR) return err_code;

    for (i = st; i <= ed; i++) MEM[i] = val;
    return NO_ERR;
}

// reset all of the memory values as 0
void reset() {
    int i;
    for (i = 0; i < MEM_SIZE; i++) MEM[i] = 0;
}

// print out the values of MEM, in the character format
void print_chars(int row) {
    int i;
    for (i = 0; i < 16; i++) {
	char now = MEM[row * 16 + i];
        if (now >= 32 && now <= 126) printf("%c", now);
        else printf(".");
    }
    printf("\n");
}

// validate one hexa string argument
int validate_one_hexstr_argument(char* arg1, int* ret1) {
    *ret1 = hexstr_to_int(arg1);
    if (*ret1 == ERR_WRONG_HEXSTR) {
	printf("ERROR: wrong hexa string to turn into int.\n"); 
	return ERR_WRONG_HEXSTR;
    }
    return NO_ERR;
}

// validate two hexa string arguments
int validate_two_hexstr_arguments(char* arg1, char* arg2, int* ret1, int* ret2) {
    if (arg1[strlen(arg1) - 1] != ',') {
	printf("ERROR: should use ',' between two arguments.\n");
	return ERR_ARGS_FORMAT;
    }
    arg1[strlen(arg1) - 1] = '\0';
    *ret1 = hexstr_to_int(arg1);
    *ret2 = hexstr_to_int(arg2);
    if (*ret1 == ERR_WRONG_HEXSTR || *ret2 == ERR_WRONG_HEXSTR) {
	printf("ERROR: wrong hexa string to turn into int.\n"); 
	return ERR_WRONG_HEXSTR;
    }
    return NO_ERR;    
}

// validate three hexa string arguments
int validate_three_hexstr_arguments(char* arg1, char* arg2, char* arg3,
				int* ret1, int* ret2, int* ret3) {
    if (arg1[strlen(arg1) - 1] != ',' || arg2[strlen(arg2) - 1] != ',') {
	printf("ERROR: should use ',' between two arguments.\n");
	return ERR_ARGS_FORMAT;
    }
    arg1[strlen(arg1) - 1] = '\0';
    arg2[strlen(arg2) - 1] = '\0';
    *ret1 = hexstr_to_int(arg1);
    *ret2 = hexstr_to_int(arg2);
    *ret3 = hexstr_to_int(arg3);
    if (*ret1 == ERR_WRONG_HEXSTR || *ret2 == ERR_WRONG_HEXSTR || *ret3 == ERR_WRONG_HEXSTR) {
	printf("ERROR: wrong hexa string to turn into int.\n"); 
	return ERR_WRONG_HEXSTR;
    }
    return NO_ERR;    
}

// validate adddress not to make underflow or overflow which leads to segmentation fault
int validate_address(int address) {
    if (address < 0 || address >= MEM_SIZE) {
	printf("ERROR: wrong address to access.\n");
	return ERR_WRONG_ADDR;	
    }
    return NO_ERR;
}

// validate a value to be in the range between 0x00 and 0xFF
int validate_value(int value) {
    if (value < 0 || value > 255 ) {
	printf("ERROR: wrong value to store.\n");
	return ERR_WRONG_VAL;
    }
    return NO_ERR;
}

// validate the addresses and the range between start and end
int validate_range(int start, int end) {
    if (validate_address(start) != NO_ERR) return ERR_WRONG_ADDR;
    if (validate_address(end) != NO_ERR) return ERR_WRONG_ADDR;
    if (start > end) {
	printf("ERROR: wrong range.\n");
	return ERR_WRONG_RANGE;
    }
    return NO_ERR;	
}
