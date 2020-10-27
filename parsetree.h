#ifndef PARSETREE_H
#define PARSETREE_H

#include <stdio.h>
#include "grammar.h"
#include "token.h"

// TYPEEXPRESSIONROW
typedef enum _basicElementTypeEnum {
    INTEGER,
    REAL,
    BOOLEAN
} basicElementTypeEnum;

struct rectArray {
    int dimensions;
    int** ranges;
    basicElementTypeEnum basicElementType;
};

struct jaggedArray {
    int dimensions;
    int** ranges;
    basicElementTypeEnum basicElementType;
    int** values; 
};

union typeExpression {
    basicElementTypeEnum primitive;
    struct rectArray;
    struct jaggedArray;
};

typedef enum _typeVariableEnum {
    PRIMITIVE,
    RECTARRAY,
    JAGGEDARRAY
} typeVariableEnum;

typedef enum _typeRectArrayEnum {
    STATIC,
    DYNAMIC,
    NOT_APPLICABLE
} typeRectArrayEnum;

typedef struct _typeExpressionRow {
    char* symbol;
    typeVariableEnum typeVariable;
    typeRectArrayEnum rectArrayType;
    union typeExpression expression;
} typeExpressionRow;

// PARSETREE
typedef struct tokenNode* parserStackNode;

struct parserStack {
    parserStackNode token;
    int terminal;
    struct _parseNode* parseTreeNode;  
    //struct parserStack* prev;  
    struct parserStack* next;
};

typedef struct parserStack* stack;

/*
OK - Symbol name
OK - Whether terminal or non terminal
OK - Type expression stored in the corresponding node (if non-leaf)
OK - Name of lexeme (if leaf node)
OK - Line number (if leaf node)
Grammar rule applied for expansion of this node while parsing (if non leaf)
Depth of node (root of the parse tree at depth 0)
*/

typedef struct _parseNode {
    struct tokenNode* token;
    int terminal;
    typeExpressionRow* typeExpression;
    int grammar_rule_idx; // 1-based index
    int depth;

    int num_child;
    struct _parseNode** children;
} parseNode;

typedef parseNode parseTree;

extern void createParseTree(parseTree *t, tokenStream *s, grammar G);

#endif // PARSETREE_H