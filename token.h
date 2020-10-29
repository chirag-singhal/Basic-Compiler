/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/


#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grammar.h"

#define NUM_TERMS 31
#define NUM_NONTERMS 45


struct tokenNode {
    int tokenID;
    char* symbol;
    int lineNumber;
};

typedef struct tokenStr {
    struct tokenNode* token;
    struct tokenStr* next;
} tokenStream;

extern void tokeniseSourcecode( char* sourceCodeFileName, tokenStream *s);

#endif // TOKEN_H