/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/

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
    /*
    int option;
    printf("Option 0: exit\nOption 1: Create parse tree\nOption 2: Traverse the parse tree to construct typeExpressionTable. Also print the type errors while
    traversing the parse tree and accessing the typeExpressionTable.\nOption 3: Print parse tree in the specified format\nOption 4: Print typeExpressionTable in the specified format.\n")
    printf("Choose an option to continue: ");
    scanf("%d", &option);
    while(option)
    {
        if(option == 1)
        {
            //create parse tree
        }
        else if(option == 2)
        {
            //traverse tree, make table and print errors
        }
        else if(option == 3)
        {
            //print parse tree in given format
        }
        else if(option == 4)
        {
            //print expression table
        }
        printf("Option 0: exit\nOption 1: Create parse tree\nOption 2: Traverse the parse tree to construct typeExpressionTable. Also print the type errors while
        traversing the parse tree and accessing the typeExpressionTable.\nOption 3: Print parse tree in the specified format\nOption 4: Print typeExpressionTable in the specified format.\n")
        printf("Choose an option to continue: ");
        scanf("%d", &option);
    }
    */
}