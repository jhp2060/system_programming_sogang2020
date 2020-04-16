#include "assembler_commands.h"

error assemble(char* filename, int token_count) {
    if (token_count != 2) return ERR_WRONG_TOKENS;
    FILE* fp = fopen(filename, "r");
    if (!fp) return ERR_NOT_A_FILE;
    int len = strlen(filename);
    if (len < 5 || strcmp(filename + len - 4, ".asm"))
	return ERR_NOT_A_ASM_FILE;      
    fclose(fp);
    return NO_ERR;
}

error pass1(FILE* fp) {
    char line[MAX_LINE_LEN];
    fgets(  
}
