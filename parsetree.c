#include "parsetree.h"
#include "token.h"
#include "grammar.h"

#define NUM_RULES 46
//push
stack push(stack s, stack head) {
    if(head==NULL) {
        head = s;
        return head;
    }
    s->next = head;
    head->prev = s;
    s->prev = NULL;
    head = s;
    return head;
}
//pop
stack pop(stack head) {
    if(head==NULL)
        return head;
        
    head = head->next;
    head->prev = NULL;
    return head;
}
//initialize stack
void createParseTree(parseTree *t, tokenStream *s, grammar G) {
    stack st = malloc(sizeof(struct parserStack));
    
    for(int i = 0; i < NUM_RULES; i++) {
        if(strcmp(t -> token -> symbol, G[i].lhs_nonterminal) == 0) {
            stack node = malloc(sizeof(struct parserStack));
            node -> token = t -> token;
            node -> terminal = 1;
            node -> prev = NULL;
            node -> next = NULL;
            push(node, st);
            break;
        }
    }

    while (s != NULL && st -> token != NULL) {
        //
        stack top = st;
        pop(st);   
        for(int i = 0; i < NUM_RULES; i++) {
             
        }
        //
        s = s -> next;
    }
}