#include "memory_commands.h"

int dump(char* start, char* end) {
    int row, col;
    int st = transform_start(start);
    int ed = transform_end(st, end);    
    int err_code = validate_start_end(st, ed);
    if (err_code != 1) return err_code;
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
    return 1;
}

int edit(char* address, char* value) {
    int addr = hexstr_to_int(address);
    int val = hexstr_to_int(value);
    if (addr == -1 || val == -1) return -1;
    if (addr < 0 || addr >= MEM_SIZE) {
	printf("ERROR(memory): wrong address to access.\n");
	return -1;
    }
    if (val < 0 || val > 255) {
	printf("ERROR: wrong value to store.\n");
	return -1;
    }
    MEM[addr] = value;
    return 1;
}

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
