#include "grammar.h"

#define NUM_RULES 44
#define MAX_SIZE 25


int isTerminal(const char* word) {
    if ('a' <= word[0] && word[0] <= 'z')
        return 0;
    return 1;
}

void readGrammar(const char* filename, grammar G) {
    FILE* grammar_fp;
    grammar_fp = fopen(filename, "r");
    if(grammar_fp == NULL) {
        printf("ERROR : GRAMMAR FILE NOT FOUND\n");
        return;
    }
    for(int i = 0; i < NUM_RULES; ++i) {
        char prod_rule[100];
        fgets(prod_rule, 100, grammar_fp);
        int len = strlen(prod_rule);
        prod_rule[len - 1] = '\0';
        
        char *token;
        token = strtok(prod_rule, " ");
        
        G[i].lhs_nonterminal = malloc(sizeof(char) * MAX_SIZE);
        strcpy(G[i].lhs_nonterminal, token);
        struct rhs_production_rule* prev_rhs_rule = NULL;
        while ((token = strtok(NULL, " ")) != NULL) {
            struct rhs_production_rule* rhs_rule = malloc(sizeof(struct rhs_production_rule));
            rhs_rule -> word = malloc(sizeof(char) * MAX_SIZE);
            strcpy(rhs_rule -> word, token);
            rhs_rule -> terminal = isTerminal(token);
            rhs_rule -> next = NULL;
            if(prev_rhs_rule == NULL)
                G[i].head = rhs_rule;
            else 
                prev_rhs_rule -> next = rhs_rule;
            prev_rhs_rule = rhs_rule;
        }
    }
    printf("Working! %s\n", G[10].head->next->next->next->next->next->next->next->word);
}