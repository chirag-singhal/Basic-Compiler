/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/

#include "grammar.h"
#include "token.h"
#include "parsetree.h"
#include "print.h"

#define NUM_RULES 68

void main(int argc, char *argv[]) {
    //int argc, char *argv[]
    grammar G = (struct production_rule*)malloc(sizeof(struct production_rule) * NUM_RULES);
    readGrammar("grammar.txt", G);
    
    tokenStream* s = (tokenStream*)malloc(sizeof(tokenStream));
    tokeniseSourcecode(argv[1], s);   //argv[1]

    parseTree* t = NULL;
    typeExpressionTable T = NULL;
    
    int option = 0;

    printf("Option 0: exit\nOption 1: Create parse tree\nOption 2: Traverse the parse tree to construct typeExpressionTable and print errors\nOption 3: Print parse tree\nOption 4: Print typeExpressionTable\n");
    printf("Choose an option to continue: ");
    scanf("%d", &option);

    while(option) {
        if(option == 1) {
            //create parse tree
            t = malloc(sizeof(struct _parseNode));
            createParseTree(t, s, G);
            printf("Parse tree created SUCCESSFULLY!!\n");
        }
        else if(option == 2) {
            //traverse tree, make table and print errors
            t = malloc(sizeof(struct _parseNode));
            createParseTree(t, s, G);
            T = malloc(sizeof(typeExpressionTableNode));
            T -> symbol = NULL;
            traverseParseTree(t, T);
        }
        else if(option == 3) {
            //print parse tree in given format
            t = malloc(sizeof(struct _parseNode));
            createParseTree(t, s, G);
            printParseTree(t);
        }
        else if(option == 4) {
            //print expression table
             t = malloc(sizeof(struct _parseNode));
            createParseTree(t, s, G);
            T = malloc(sizeof(typeExpressionTableNode));
            traverseParseTree(t, T);
            printTypeExpressionTable(T);
        }

        printf("\n\nOption 0: exit\nOption 1: Create parse tree\nOption 2: Traverse the parse tree to construct typeExpressionTable and print errors\nOption 3: Print parse tree\nOption 4: Print typeExpressionTable\n");
        printf("Choose an option to continue: ");
        scanf("%d", &option);
    }
}


