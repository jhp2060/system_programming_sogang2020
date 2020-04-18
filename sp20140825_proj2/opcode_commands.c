#include "opcode_commands.h"

// print out the opcode for the mnemonic
error opcode(char* mnemonic, int token_count) {
    if (token_count != 2) return ERR_WRONG_TOKENS;
    opcode_node* now = get_opcode(mnemonic);
    if (!now) return ERR_WRONG_MNEMONIC;
    printf("opcode is %02X\n", now->opcode);
    return NO_ERR;
}

// print out the opcde list in the optab 
error opcodelist(int token_count) {
    if (token_count != 1) return ERR_WRONG_TOKENS;
    int i;
    for (i = 0; i < OPTAB_SIZE; i++) {
	printf("%d : ", i);
	opcode_node* now = OPTAB[i];
	while (now != NULL) {
	    printf("[%s,%02X]", now->mnemonic, now->opcode);
	    now = now->next;
	    if (now == NULL) break;
	    printf(" -> ");
	}
	printf("\n");
    }
    return NO_ERR;
}

// read the opcodes from a file and make them into opcode table
void init_optab(char* filename) {
    int i, optab_index;
    FILE *fp = fopen(filename, "r");
    opcode_node* new = malloc(sizeof(opcode_node));
    
    // initialize OPTAB
    for (i = 0; i < OPTAB_SIZE; i++) OPTAB[i] = NULL;
    while(fscanf(fp, "%X%s%s", &new->opcode, new->mnemonic, new->format) != EOF) {
	optab_index = get_optab_index(new->mnemonic);
	new->next = OPTAB[optab_index];
	OPTAB[optab_index] = new;
	new = malloc(sizeof(opcode_node));
    }
    free(new);
    fclose(fp);
}

// hash funciton
int get_optab_index(char* mnemonic) {
    int i, ret = 0, len = strlen(mnemonic);
    for (i = 0; i < len; i++) ret += mnemonic[i];
    if (ret < 0) ret *= -1;
    return ret % OPTAB_SIZE;
}

// free the dynamically allocated memories
void free_optab(void) {
    int i;
    opcode_node *now, *next;
    for (i = 0; i < OPTAB_SIZE; i++) {
	now = OPTAB[i];
	while (now != NULL) {
	    next = now->next;
	    free(now);
	    now = next;   
	}	
    }
}

// find opcode
opcode_node* get_opcode(char* mnemonic) {
    int index = get_optab_index(mnemonic);
    opcode_node* now = OPTAB[index];
    while (now != NULL) {	// find the opcode
	if (strcmp(now->mnemonic, mnemonic) == 0)
	    return now;
	now = now->next;
    }
    return NULL; 
}
