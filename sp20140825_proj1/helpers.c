#include "helpers.h"

// return the boolean value expressing whether the norm is same with either one of the operands
int strcmp_twice(char* norm, char* op1, char* op2) {
    return (strcmp(norm, op1) == 0 || strcmp(norm, op2) == 0);
}

// transform the hexa string into int vlaue
int hexstr_to_int(char* str) {
    int i, ret = 0, now, mult = 1;
    int len = strlen(str);
    for (i = len - 1; i >= 0; i--) {
        if (str[i] >= '0' && str[i] <= '9') now = str[i] - '0';
        else if (str[i] >= 'A' && str[i] <= 'F')	// for the uppercase
            now = str[i] - 'A' + 10;
	else if (str[i] >= 'a' && str[i] <= 'f')	// for the lowercase
	    now = str[i] - 'a' + 10;
        else return ERR_WRONG_HEXSTR;			// wrong format of hexa string
        ret += (now * mult);
        mult *= 16;
    }
    return ret;
}
