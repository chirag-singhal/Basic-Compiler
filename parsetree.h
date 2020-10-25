#ifndef PARSETREE_H
#define PARSETREE_H

#include <stdio.h>
#include "token.h"


typedef struct tokenNode* parserStackNode;

struct parserStack {
    parserStackNode token;
    int terminal;
    parserStackNode prev;  
    parserStackNode next;
};

typedef struct parserStack* stack;

typedef struct _parseNode {
    struct tokenNode* token;
    int num_child;
    struct _parseNode* children;
} parseNode;

typedef parseNode parseTree;

extern void createParseTree(parseTree *t, tokenStream *s, grammar G);

#endif // PARSETREE_H