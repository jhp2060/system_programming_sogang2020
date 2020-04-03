#include "memory_commands.h"

// print out the memory between start and end
int dump(char* start, char* end) {
    int row, col;
    int st, ed, err_code;
    
    // argument parsing and validation
    switch (TOKEN_COUNT) {
	case 1: // no arguments
	    st = LAST_ADDR + 1 < MEM_SIZE ? LAST_ADDR + 1 : 0;
	    ed = st + 16 * 10 - 1 < MEM_SIZE ? st + 16 * 10 - 1 : MEM_SIZE - 1;
	    break;
	case 2: // 1 argument
	    err_code = validate_one_hexstr_argument(start, &st);
	    if (err_code != 1) return err_code;
	    ed = st + 16 * 10 - 1 < MEM_SIZE ? st + 16 * 10 - 1 : MEM_SIZE - 1;
	    break;	    
	case 3: // 2 arguments
	    err_code = validate_two_hexstr_arguments(start, end, &st, &ed);
	    if (err_code != 1) return err_code;
	    break;
	default:
	   return -1;
    }
    err_code = validate_range(st, ed);
    if (err_code != 1) return err_code;

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
    return 1;
}

// edit the value of memory at the address
int edit(char* address, char* value) {
    if (address[strlen(address) - 1] != ',') {
	printf("ERROR: should use ',' between two arguments.\n");
	return -1;
    }
    address[strlen(address) - 1] = '\0';
    int addr = hexstr_to_int(address);
    int val = hexstr_to_int(value);
    if (addr == -1 || val == -1) return -1;
    if (addr < 0 || addr >= MEM_SIZE) {
	printf("ERROR: wrong address to access.\n");
	return -1;
    }
    if (val < 0 || val > 255) {
	printf("ERROR: wrong value to store.\n");
	return -1;
    }
    MEM[addr] = val;
    return 1;
}

// fill the memory spaces between start and end with a value
int fill(char* start, char* end, char* value) {
    if (start[strlen(start) - 1] != ',' || end[strlen(end) -1] != ',') {
	printf("ERROR: should use ',' between two arguments.\n");
	return -1;
    }
    start[strlen(start) - 1] = ',';
    end[strlen(end) - 1] = ',';
    int st = hexstr_to_int(start), ed = hexstr_to_int(end);
    int val = hexstr_to_int(value);
    if (st == -1 || ed == -1 || val == -1) {
    }
    return 1;
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

int transform_start(char* start) {
    if (TOKEN_COUNT == 1) // no arguments -> automatical addressing
	return LAST_ADDR + 1 < MEM_SIZE ? LAST_ADDR + 1 : 0; 
    if (TOKEN_COUNT == 2) return hexstr_to_int(start); // one argument
    if (TOKEN_COUNT == 3) { // two arguments
    	if (start[strlen(start)- 1] != ',') return -2; // ',' must be next to start argument
    	start[strlen(start) - 1] = '\0';
    	return hexstr_to_int(start);
    }
    return -1;
}

int transform_end(int start, char* end) {
    if (TOKEN_COUNT == 1 || TOKEN_COUNT == 2) // 0, 1 argument -> automatical addressing
	return start + 16 * 10 - 1 < MEM_SIZE ? start + 16 * 10 - 1 : MEM_SIZE - 1;
    if (TOKEN_COUNT == 3) return hexstr_to_int(end); // 2 arguments
    return -1;
}

// check the validity of start and end of int value
// -1 : normal_err_code, -2 : err_code to print out err_msg
int validate_start_end(int start, int end) {
    if (start == -2) {
	printf("ERROR: should use ',' between two arguments.\n");
	return -1;
    }
    if (start == -1 || end == -1) {
	printf("ERROR: wrong format for hex.\n"); 
	return -1;  	// something wrong happened in transformation
    }
    if (start > end) {
	printf("ERROR: wrong range.\n"); 
	return -1;
    }
    if (start < 0 || start >= MEM_SIZE || end < 0 || end >= MEM_SIZE) {
	printf("ERROR: wrong address to access.\n");
	return -1; // wrong address
    }
    if (TOKEN_COUNT == 1) LAST_ADDR = end;
    return 1;
}

int validate_one_hexstr_argument(char* arg1, int* ret1) {
    *ret1 = hexstr_to_int(arg1);
    if (*ret1 == -1) {
	printf("ERROR: wrong hexa string to turn into int.\n"); 
	return -1;
    }
    return 1;
}

int validate_two_hexstr_arguments(char* arg1, char* arg2, int* ret1, int* ret2) {
    if (arg1[strlen(arg1) - 1] != ',') {
	printf("ERROR: should use ',' between two arguments.\n");
	return -2;
    }
    arg1[strlen(arg1) - 1] = '\0';
    *ret1 = hexstr_to_int(arg1);
    *ret2 = hexstr_to_int(arg2);
    if (*ret1 == -1 || *ret2 == -1) {
	printf("ERROR: wrong hexa string to turn into int.\n"); 
	return -1;
    }
    return 1;    
}

int validate_three_hexstr_arguments(char* arg1, char* arg2, char* arg3,
				int* ret1, int* ret2, int* ret3) {
    if (arg1[strlen(arg1) - 1] != ',' || arg2[strlen(arg2) - 1] != ',') {
	printf("ERROR: should use ',' between two arguments.\n");
	return -2;
    }
    arg1[strlen(arg1) - 1] = '\0';
    arg2[strlen(arg2) - 1] = '\0';
    *ret1 = hexstr_to_int(arg1);
    *ret2 = hexstr_to_int(arg2);
    *ret3 = hexstr_to_int(arg3);
    if (*ret1 == -1 || *ret2 == -1 || *ret3 == -1) {
	printf("ERROR: wrong hexa string to turn into int.\n"); 
	return -1;
    }
    return 1;    
}

int validate_address(int address) {
    if (address < 0 || address >= MEM_SIZE) {
	printf("ERROR: wrong address to access.\n");
	return -3;	
    }
    return 1;
}

int validate_value(int value) {
    if (value < 32 || value > 126 ) { // range between 0x20 and 0x7E
	printf("ERROR: wrong value to store.\n");
	return -4;
    }
    return 1;
}

int validate_range(int start, int end) {
    if (validate_address(start) != 1) return -3;
    if (validate_address(end) != 1) return -3;
    if (start > end) {
	printf("ERROR: wrong range.\n");
	return -5;
    }
    return 1;	
}
