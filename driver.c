#include "grammar.h"
#include "token.h"
#include "parsetree.h"

#define NUM_RULES 65

void main() {
    grammar G = (struct production_rule*)malloc(sizeof(struct production_rule) * NUM_RULES);
    readGrammar("grammar.txt", G);
    
    tokenStream* s = (tokenStream*)malloc(sizeof(tokenStream));
    tokeniseSourcecode("sourcecode.txt", s);
    
    parseTree* t = malloc(sizeof(struct _parseNode));
    createParseTree(t, s, G);
}