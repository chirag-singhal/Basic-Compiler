/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/

#ifndef PRINT_H
#define PRINT_H

#include "parsetree.h"
#include "typeExpression.h"


extern void printTypeExpression(typeExpressionRow* row);

extern void printParseTree(parseTree* t);

extern void printTypeExpressionTable(typeExpressionTable T);

#endif // PRINT_H