#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_TERMS 31
#define NUM_NONTERMS 43

typedef enum {
    PROGRAM,    
    OP_CP,
    OCB,
    CCB,
    DECLARE,
    COLON,
    LIST,
    OF,
    VARIABLES,
    INTEGER,
    BOOLEAN,
    REAL,
    SEMI_COLON,
    ARRAY,
    JAGGED,
    OSB,
    DOUBLE_DOT,
    CSB,
    R1,
    SIZE,
    VALUES,
    ASSIGN_OP,
    OR_LOGIC_OP,
    AND_LOGIC_OP,
    ADD_OP,
    SUB_OP,
    MUL_OP,
    DIV_OP,
    NUM,
    VAR_ID,
    END
} termid;

typedef enum {
    start,
    stmts,
    declarativeStmts,
    declarativeStmtsOptional,
    assignmentStmts,
    assignmentStmtsOptional,
    declarativeStmt,
    varDeclarePhase,
    varOrListDeclare,
    listVarId,
    listVarIdOptional,
    type,
    specificDeclarativePhase,
    primitiveDeclarativePhrase,
    rectArrayDeclarativePhrase,
    jaggedArrayDeclarativePhrase,
    listDim,
    listDimOptional,
    idx,
    listEmptyDim,
    listEmptyDimOptional,
    jaggedArrayInitialisation,
    jaggedArrayInitializationPhrase,
    jaggedArrayInitializationOptional,
    listIntSemiColon,
    listIntSemiColonOptional,
    listInt,
    listIntOptional,
    assignmentStmt,
    expression,
    element,
    elementDimOptional,
    orLogicExpression,
    orLogicExpressionOptional,
    andLogicExpression,
    andLogicExpressionOptional,
    addExpression,
    addExpressionOptional,
    adOps,
    mulExpression,
    mulExpressionOptional,
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