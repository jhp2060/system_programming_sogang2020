#include "opcode_commands.h"

int opcode(char* mnemonic, int token_count) {
    if (token_count != 2) return -6; // wrong_token

}

void init_hash_table(char* filename) {
    int i, hash_index;
    FILE *fp = fopen(filename, "r");
    hash* new = malloc(sizeof(hash));
    
    // initialize HASH_TABLE
    for (i = 0; i < HASH_TABLE_SIZE; i++) HASH_TABLE[i] = NULL;
    while(fscanf(fp, "%s%s%s", new->opcode, new->mneumonic, new->format) != EOF) {
	hash_index = get_hash_index(new);
	new->next = HASH_TABLE[hash_index];
	HASH_TABLE[hash_index] = new;
	new = malloc(sizeof(hash));
    }
    free(new);
}

int get_hash_index(hash* node) {
    int ret = hexstr_to_int(node->opcode);
    if (ret % 16 == 0 || ret % 16 == 8) ret++;  // make index with odd number
    return ret % 20;
}

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
