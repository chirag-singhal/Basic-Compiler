#ifndef TYPEEXPRESSION_H
#define TYPEEXPRESSION_H

#include "parsetree.h"


typedef struct _typeExpressionTableNode {
    char* symbol;
    typeExpressionRow* row;
    struct _typeExpressionTableNode* next;
    struct _typeExpressionTableNode* prev;
} typeExpressionTableNode;

typedef typeExpressionTableNode* typeExpressionTable;

extern void push_typeExpTable(char* symbol, typeExpressionRow* row, typeExpressionTable T);

extern typeExpressionRow* search_typeExpTable(char* symbol, typeExpressionTable T);

extern void traverseParseTree( parseTree *t, typeExpressionTable T );

#endif // TYPEEXPRESSION_H