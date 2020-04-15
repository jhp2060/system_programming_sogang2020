#include "opcode_commands.h"

// print out the opcode for the mnemonic
error opcode(char* mnemonic, int token_count) {
    if (token_count != 2) return ERR_WRONG_TOKENS;
    int index = get_hash_index(mnemonic);
    hash* now = HASH_TABLE[index];
    while (now != NULL) {	// find the opcode
	if (strcmp(now->mnemonic, mnemonic) == 0) {
	    printf("opcode is %s\n", now->opcode);
	    return NO_ERR;
	}	
	now = now->next;
    }   
    return ERR_WRONG_MNEMONIC;
}

// print out the opcde list in the hash table 
error opcodelist(int token_count) {
    if (token_count != 1) return ERR_WRONG_TOKENS;
    int i;
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
	printf("%d : ", i);
	hash* now = HASH_TABLE[i];
	while (now != NULL) {
	    printf("[%s,%s]", now->mnemonic, now->opcode);
	    now = now->next;
	    if (now == NULL) break;
	    printf(" -> ");
	}
	printf("\n");
    }
    return NO_ERR;
}

// read the opcodes from a file and make them into hash table
void init_hash_table(char* filename) {
    int i, hash_index;
    FILE *fp = fopen(filename, "r");
    hash* new = malloc(sizeof(hash));
    
    // initialize HASH_TABLE
    for (i = 0; i < HASH_TABLE_SIZE; i++) HASH_TABLE[i] = NULL;
    while(fscanf(fp, "%s%s%s", new->opcode, new->mnemonic, new->format) != EOF) {
	hash_index = get_hash_index(new->mnemonic);
	new->next = HASH_TABLE[hash_index];
	HASH_TABLE[hash_index] = new;
	new = malloc(sizeof(hash));
    }
    free(new);
}

// hash funciton
int get_hash_index(char* mnemonic) {
    int i, ret = 0, len = strlen(mnemonic);
    for (i = 0; i < len; i++) ret += mnemonic[i];
    if (ret < 0) ret *= -1;
    return ret % HASH_TABLE_SIZE;
}

// free the dynamically allocated memories
void free_hash_table(void) {
    int i;
    hash *now, *next;
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
	now = HASH_TABLE[i];
	while (now != NULL) {
	    next = now->next;
	    free(now);
	    now = next;   
	}	
    }
}
