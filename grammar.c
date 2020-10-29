/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/


#include "grammar.h"

#define NUM_RULES 68
#define MAX_SIZE 50


char* known_symbols[] = {
    "program",
    "()",
    "{",
    "}",
    "declare",
    ":",
    "list",
    "of",
    "variables",
    "integer",
    "boolean",
    "real",
    ";",
    "array",
    "jagged",
    "[",
    "..",
    "]",
    "R1",
    "size",
    "values",
    "=",
    "|||",
    "&&&",
    "+",
    "-",
    "*",
    "/"
};

char* term_grammar[] = {
    "PROGRAM",
    "OP_CP",
    "OCB",
    "CCB",
    "DECLARE",
    "COLON",
    "LIST",
    "OF",
    "VARIABLES",
    "INTEGER",
    "BOOLEAN",
    "REAL",
    "SEMI_COLON",
    "ARRAY",
    "JAGGED",
    "OSB",
    "DOUBLE_DOT",
    "CSB",
    "R1",
    "SIZE",
    "VALUES",
    "ASSIGN_OP",
    "OR_LOGIC_OP",
    "AND_LOGIC_OP",
    "ADD_OP",
    "SUB_OP",
    "MUL_OP",
    "DIV_OP",
    "NUM",
    "VAR_ID",
    "EPSILON"
};

char* nonterm_grammar[] = {
    "start",
    "stmts",
    "declarativeStmts",
    "declarativeStmtsOptional",
    "assignmentStmts",
    "assignmentStmtsOptional",
    "declarativeStmt",
    "varDeclarePhrase",
    "varOrListDeclare",
    "listVarId",
    "listVarIdOptional",
    "type",
    "specificDeclarativePhrase",
    "primitiveDeclarativePhrase",
    "rectArrayDeclarativePhrase",
    "jaggedArrayDeclarativePhrase",
    "listDim",
    "listDimOptional",
    "idx",
    "listEmptyDim",
    "listEmptyDimOptional",
    "jaggedArrayInitialization",
    "jaggedArrayInitializationPhrase",
    "jaggedArrayInitializationOptional",
    "listIntSemiColon",
    "listIntSemiColonOptional",
    "listInt",
    "listIntOptional",
    "assignmentStmt",
    "expression",
    "element",
    "elementDimOptional",
    "listIdx",
    "listIdxOptional",
    "orLogicExpression",
    "orLogicExpressionOptional",
    "andLogicExpression",
    "andLogicExpressionOptional",
    "addExpression",
    "addExpressionOptional",
    "addOps",
    "mulExpression",
    "mulExpressionOptional",
    "mulOps",
    "term"
};

int getToken(char* word) {
    for (int i = 0; i < sizeof(known_symbols)/sizeof(known_symbols[0]); ++i) {
        if (strcmp(word, known_symbols[i]) == 0) {
            return i;
        }
    }
    // check for NUM
    int len = strlen(word);
    int valid_num = 1;
    for(int i = 0; i < len; i++) {
        if(word[i] < '0' || word[i] > '9') {
            valid_num = 0;
            break;
        }
    }
    if(valid_num)
        return NUM;
    // check for VAR_ID
    if (len > 20 || !((word[0] >= 'a' && word[0] <= 'z') || word[0] =='_' || (word[0] >= 'A' && word[0] <= 'Z')))
        return -1;
    for (int i = 1; i < len; ++i) {
        if ((word[i] < 'a' || word[i] > 'z') && word[i] != '_' && (word[i] < 'A' || word[i] > 'Z') && (word[i] < '0' || word[i] > '9'))
            return -1;
    }
    return VAR_ID;
}

int getTerm(char* word) {
    for (int i = 0; i < sizeof(term_grammar)/sizeof(term_grammar[0]); ++i) {
        if (strcmp(word, term_grammar[i]) == 0) {
            return i;
        }
    }
    // if no non-terminal matches
    return -1;
}

int getNonTerm(char* word) {
    for (int i = 0; i < sizeof(nonterm_grammar)/sizeof(nonterm_grammar[0]); ++i) {
        if (strcmp(word, nonterm_grammar[i]) == 0) {
            return i;
        }
    }
    // if no non-terminal matches
    return -1;
}

int isTerminal(const char* word) {
    if ('a' <= word[0] && word[0] <= 'z')
        return 0;
    return 1;
}

void readGrammar(const char* filename, grammar G) {
    FILE* grammar_fp;
    grammar_fp = fopen(filename, "r");
    if(grammar_fp == NULL) {
        printf("ERROR : GRAMMAR FILE NOT FOUND\n");
        return;
    }
    for(int i = 0; i < NUM_RULES; ++i) {
        char prod_rule[150];
        fgets(prod_rule, 150, grammar_fp);
        int len = strlen(prod_rule);
        if(prod_rule[len - 1] == '\n')
            prod_rule[len - 1] = '\0';
        
        char *token;
        token = strtok(prod_rule, " ");
        
        G[i].lhs_nonterminal = malloc(sizeof(char) * MAX_SIZE);
        strcpy(G[i].lhs_nonterminal, token);
        G[i].nontermid = getNonTerm(token);
        struct rhs_production_rule* prev_rhs_rule = NULL;
        while ((token = strtok(NULL, " \0")) != NULL) {
            struct rhs_production_rule* rhs_rule = malloc(sizeof(struct rhs_production_rule));
            rhs_rule -> word = malloc(sizeof(char) * MAX_SIZE);
            strcpy(rhs_rule -> word, token);
            rhs_rule -> terminal = isTerminal(token);
            if (rhs_rule -> terminal)
                rhs_rule -> tokenId = getTerm(rhs_rule -> word);
            else
                rhs_rule -> tokenId = getNonTerm(rhs_rule -> word);
            rhs_rule -> next = NULL;
            if(prev_rhs_rule == NULL)
                G[i].head = rhs_rule;
            else 
                prev_rhs_rule -> next = rhs_rule;
            prev_rhs_rule = rhs_rule;
        }
    }
}