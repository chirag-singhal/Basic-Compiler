#ifndef GRAMMAR_H
#define GRAMMAR_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>


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
    EPSILON
} termid;

typedef enum {
    start,
    stmts,
    declarativeStmts,
    declarativeStmtsOptional,
    assignmentStmts,
    assignmentStmtsOptional,
    declarativeStmt,
    varDeclarePhrase,
    varOrListDeclare,
    listVarId,
    listVarIdOptional,
    type,
    specificDeclarativePhrase,
    primitiveDeclarativePhrase,
    rectArrayDeclarativePhrase,
    jaggedArrayDeclarativePhrase,
    listDim,
    listDimOptional,
    idx,
    listEmptyDim,
    listEmptyDimOptional,
    jaggedArrayInitialization,
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
    addOps,
    mulExpression,
    mulExpressionOptional,
    mulOps,
    term
} nontermid;

struct rhs_production_rule {
    char* word;
    struct rhs_production_rule* next;
    int terminal;
    int tokenId;
};

struct production_rule {
    char* lhs_nonterminal;
    int nontermid;
    struct rhs_production_rule* head; 
};

typedef struct production_rule* grammar;

extern int getTerm(char* word);
extern int getNonTerm(char* word);
extern void readGrammar(const char* filename, grammar G);
extern int getToken(char* word);

#endif // GRAMMAR_H