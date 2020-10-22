#include "grammar.h"

void main() {
    grammar G = (struct production_rule*)malloc(sizeof(struct production_rule) * NUM_RULES);
    readGrammar("grammar.txt", G);
}