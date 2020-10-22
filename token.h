#include <stdio.h>

typedef enum {
    OP,
    CP,
    OCB,
    CCB,
    SO,
    SC,
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
    VARIABLES,
    DOUBLE_DOT,
    ADD,
    SUB,
    MUL,
    DIV,
    LOGICAL_OR,
    LOGICAL_AND,
    NUM,
    VAR_NAME,
} tokenid;

struct tokenNode {
    int tokenID;
    char* known_symbol;
    int lineNumber;
};

typedef struct tokenStr {
    struct tokenNode* token;
    struct tokenNode* next;
} tokenStream;

extern int getToken(char* word);

extern void tokeniseSourcecode( char* sourceCodeFileName, tokenStream *s);