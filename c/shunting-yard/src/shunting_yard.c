#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shunting_yard.h"

static const uint8_t precedence[] = {
    ['+'] = 1, ['-'] = 2, ['*'] = 3, ['/'] = 3
};

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

static bool parse_operator(char* formula, int8_t *operator, char **endptr) {
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

typedef struct SH {
    double numstack[256];
    size_t numstack_len;

    char opstack[256];
    size_t opstack_len;
} SH;

#define ST_HEAD(sh, stack) ((sh).stack[(sh).stack##_len-1])
#define ST_POP(sh, stack) ((sh).stack[--(sh).stack##_len])
#define ST_PUSH(sh, stack, value) ((sh).stack[(sh).stack##_len++] = value)
#define ST_LEN(sh, stack) ((sh).stack##_len)

static bool popeval_once(struct SH *sh) {
    if(ST_LEN(*sh, numstack) < 2 || ST_LEN(*sh, opstack) < 1) {
        return false;
    }

    char operator = ST_POP(*sh, opstack);
    const double operand = ST_POP(*sh, numstack);
    switch(operator) {
        case '+':
            ST_HEAD(*sh, numstack) += operand;
            break;
        case '-':
            ST_HEAD(*sh, numstack) -= operand;
            break;
        case '*':
            ST_HEAD(*sh, numstack) *= operand;
            break;
        case '/':
            ST_HEAD(*sh, numstack) /= operand;
            break;
    }
    return true;
}

bool SH_trySolve(char *formula, double *result) {
    SH sh = {0};

    char *ptr = formula;
    char *next_ptr;
    int8_t operator;
    while(*ptr != '\0') {
        if(parse_number(ptr, &sh.numstack[sh.numstack_len], &next_ptr)) {
            sh.numstack_len++;
        } else if(parse_operator(ptr, &operator, &next_ptr)) {
            if(ST_LEN(sh, numstack) == 0) {
                return false;
            }

            if(ST_LEN(sh, opstack) > 0 && precedence[operator] < precedence[(uint8_t)ST_HEAD(sh, opstack)]) {
                while(ST_LEN(sh, opstack) > 0 && precedence[operator] < precedence[(uint8_t)ST_HEAD(sh, opstack)]) {
                    bool success = popeval_once(&sh);

                    if(!success) {
                        return false;
                    }
                }
            }
            ST_PUSH(sh, opstack, operator);
        } else {
            return false;
        }

        ptr = next_ptr;
    }

    while(ST_LEN(sh, opstack) > 0) {
        bool success = popeval_once(&sh);

        if(!success) {
            return false;
        }
    }
    
    if(ST_LEN(sh, numstack) == 1 && ST_LEN(sh, opstack) == 0) {
        *result = ST_POP(sh, numstack);
        return true;
    } else {
        return false;
    }
}