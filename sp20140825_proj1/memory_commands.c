#include "memory_commands.h"

void dump(int start, int end) {
    int i, j;
    int st_row = start / 16, st_col = start % 16;
    int ed_row = end / 16, ed_col = end % 16;
    
    // print out the first line
    print_address(st_row);
    print_bytes(st_row, st_col, 15);
    print_chars(st_row); 

    // print out the lines in middle
    for (i = st_row + 1; i < ed_row; i++) {
	print_address(i);
	print_bytes(i, 0, 16);
	print_chars(i);
    }

    // print out the last line
    print_address(ed_row);
    print_bytes(ed_row, 0, ed_col);
    print_chars(ed_row);
}

void print_address(int row) {
    printf("%05X  ", row * 16);
}

void print_bytes(int row, int st, int ed) {
    int i;
    for (i = st; i < ed; i++) printf("%02X ", MEM[row][i]);
    printf(";  ");
}

void print_chars(int row) {
    int i;
    for (i = 0; i < 16; i++) {
	char now = MEM[row][i];
        if (now >= 32 && now <= 126) printf("%c", now);
        else printf(".");
    }
    printf("\n");
}
