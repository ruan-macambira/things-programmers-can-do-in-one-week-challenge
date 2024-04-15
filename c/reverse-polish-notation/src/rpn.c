#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "rpn.h"

static bool parse_number(char* formula, double *number, char **endptr) {
    *number = strtod(formula, endptr);

    return formula != *endptr;
}

static bool ignore_whitespace(char* formula, char** endparse) {
    char *ptr = formula;
    while(*ptr == ' ') {
        ptr++;
    }

    if(*ptr != '\0') {
        *endparse = ptr;
        return true;
    }

    return false;
}

static bool parse_operator(char* formula, char *operator, char **endptr) {
    char *ptr = formula;
    if(!ignore_whitespace(formula, &ptr)) {
        return false;
    }

    switch(ptr[0]) {
        case '+': case '-': case '*': case '/':
            *operator = ptr[0];
            *endptr = ptr+1;
            return true;
        default:
            return false;
    }
}

bool RPN_trySolve(char* const formula, double *result) {
    double stack[256] = {0};
    uint8_t stacksize = 0;
    char operator;

    if(result == NULL) {
        return false;
    }

    char* ptr = formula;
    char *ptr_next;

    while(*ptr != '\0') {
        if(parse_number(ptr, &stack[stacksize], &ptr_next)) {
            ptr = ptr_next;
            ++stacksize;
        } else if(parse_operator(ptr, &operator, &ptr_next)) {
            if(stacksize == 0) {
                return false;
            }
            ptr = ptr_next;

            if(stacksize < 2) {
                return false;
            }

            switch(operator) {
                case '+':
                    stack[stacksize-2] += stack[stacksize-1];
                    break;
                case '-':
                    stack[stacksize-2] -= stack[stacksize-1];
                    break;
                case '*':
                    stack[stacksize-2] *= stack[stacksize-1];
                    break;
                case '/':
                    stack[stacksize-2] /= stack[stacksize-1];
                    break;
            }
            --stacksize;
        } else {
            return false;
        }
    }

    if(stacksize != 1) {
        return false;
    }

    *result = stack[0];
    return true;
}