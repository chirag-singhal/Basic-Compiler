#include "typeExpression.h"


void handleDeclarativeStmt(parseNode* t) {
    /*
        if varOrListDeclare, check first child
        if VAR_ID, add only 1 row
        else add multiple 
    */
   // t -> children[1] Specific Declarative Phrase
    if(t -> children[1] -> children[0] -> token -> tokenID == primitiveDeclarativePhrase);
        //Primitive
    else if(t -> children[1] -> children[0] -> token -> tokenID == rectArrayDeclarativePhrase);
        //RectArray INTEGER
    else
        //jaggedArray INTEGER
}

void traverseParseTree(parseTree *t, typeExpressionTable T) {
    /*
        start -> stmts
    */
    // declarativeStmts
    parseNode* temp = t -> children[0] -> children[0]; // this points to "declarativeStmts"
    while (temp -> num_child == 2) {
        //Cannot be epsilon
        handleDeclarativeStmt(temp -> children[0]);
        temp = temp -> children[1];
    }
   //

   // assignmentStmts
    temp = t -> children[0] -> children[1]; // this points to "assignmentStmts"
   //
}