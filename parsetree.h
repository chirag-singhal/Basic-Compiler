/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/


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

union rectArrayIndexUnion {
    int intIndex;
    char* varIdIndex; 
};

typedef struct _rectArrayIndex {
    int isVarId;
    union rectArrayIndexUnion value;
} rectArrayIndex;

struct rectArray {
    int dimensions;
    rectArrayIndex** ranges;
    basicElementTypeEnum basicElementType;
};

union jaggedArrayR2Dims {
    int* range_2d;
    int** range_3d;      //first element of each row has its size
};

struct jaggedArray {
    int dimensions;
    int R1_range[2];
    union jaggedArrayR2Dims R2_ranges;
    basicElementTypeEnum basicElementType;
};

union typeExpression {
    basicElementTypeEnum primitive;
    struct rectArray rectArray;
    struct jaggedArray jaggedArray;
};

typedef enum _typeVariableEnum {
    PRIMITIVE,
    RECTARRAY,
    JAGGEDARRAY,
    ERROR
} typeVariableEnum;

typedef enum _typeRectArrayEnum {
    STATIC,
    DYNAMIC,
    NOT_APPLICABLE
} typeRectArrayEnum;

typedef struct _typeExpressionRow {
    typeVariableEnum typeVariable;
    typeRectArrayEnum rectArrayType;
    union typeExpression expression;
} typeExpressionRow;

// PARSETREE
typedef struct tokenNode* parserStackNode;

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

struct parserStack {
    parserStackNode token;
    int terminal;
    struct _parseNode* parseTreeNode;  
    //struct parserStack* prev;  
    struct parserStack* next;
};

typedef struct parserStack* stack;

extern void createParseTree(parseTree *t, tokenStream *s, grammar G);

extern stack pop(stack head);

extern stack push(stack s, stack head);

extern void freeStackNode(stack node);

#endif // PARSETREE_H