#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_TERMS 28
#define NUM_NONTERMS 43

typedef enum {
    OP_CP,    
    OCB,
    CCB,
    OSB,
    CSB,
    COLON,
    SEMI_COLON,
    PROGRAM,
    INTEGER,
    BOOLEAN,
    REAL,
    DECLARE,
    LIST,
    OF,
    ARRAY,
    JAGGED,
    R1,
    VARIABLES,
    DOUBLE_DOT,
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,
    OR_LOGIC_OP,
    AND_LOGIC_OP,
    ASSIGN_OP,
    NUM,
    VAR_ID,
} termid;

typedef enum {
    start,
    stmts,
    declarativeStmts,
    assignmentStmts,
    declarativeStmt,
    declarativeStmtsOptional,
    assignmentStmt,
    assignmentStmtsOptional,
    varDeclarePhrase,
    specificDeclarativePhrase,
    varOrListDeclare,
    listVarId,
    listVarIdOptional,
    type,
    primitiveDeclarativePhrase,
    rectArrayDeclarativePhrase,
    jaggedArrayDeclarativePhrase,
    jaggedArrayInitialization,
    listDim,
    listDimOptional,
    listEmptyDim,
    listEmptyDimOptional,
    index,
    jaggedArrayInitializationPhrase,
    jaggedArrayInitializationOptional,
    listIntSemiColon,
    listIntSemiColonOptional,
    listInt,
    listIntOptional,
    element,
    elementDimOptional,
    expression,
    orLogicExpression,
    orLogicExpressionOptional,
    andLogicExpression,
    andLogicExpressionOptional,
    addExpression,
    addExpressionOptional,
    mulExpression,
    mulExpressionOptional,
    addOps,
    mulOps,
    term
} nontermid;

struct tokenNode {
    int tokenID;
    char* symbol;
    int lineNumber;
};

typedef struct tokenStr {
    struct tokenNode* token;
    struct tokenStr* next;
} tokenStream;

extern int getToken(char* word);

extern void tokeniseSourcecode( char* sourceCodeFileName, tokenStream *s);

#endif // TOKEN_H