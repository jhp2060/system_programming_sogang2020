#include "opcode_commands.h"


void init_hash_table(char* filename) {
    FILE *fp = fopen(filename, "r");

    hash* new = malloc(sizeof(hash));
    while(fscanf(fp, "%s%s%s", new->opcode, new->mneumonic, new->format) != EOF);
}

void free_hash_table(void) {
    int i;
    hash* now, next;
    for (i = 0; i < HASH_TABLE_SIZE; i++) {
	now = HASH_TABLE[i];
	while (now != NULL) {
	    next = now->next;
	    free(now);
	    now = next;   
	}	
    }
}
