#include<stdio.h>
#include<string.h>
#include <stdlib.h>

#define NUM_RULES 44
#define MAX_SIZE 25


struct rhs_production_rule {
    char* word;
    struct rhs_production_rule* next;
    int terminal;
};

struct production_rule {
    char* lhs_nonterminal;
    struct rhs_production_rule* head; 
};

typedef struct production_rule* grammar;

extern void readGrammar(const char* filename, grammar G);