#include "token.h"
#define MAX_SIZE 20


char* known_symbols[] = {
    "(",
    ")",
    "{",
    "}",
    "[",
    "]",
    ":",
    ";",
    "program",
    "integer",
    "boolean",
    "real",
    "declare",
    "list",
    "of",
    "array",
    "jagged",
    "variables",
    "..",
    "+",
    "-",
    "*",
    "/",
    "|||",
    "&&&"
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
    // check for VAR_NAME
    if (len > 20 || !((word[0] >= 'a' && word[0] <= 'z') || word[0] =='_' || (word[0] >= 'A' && word[0] <= 'Z')))
        return -1;
    for (int i = 1; i < len; ++i) {
        if ((word[0] < 'a' || word[0] > 'z') && word[0] != '_' && (word[0] < 'A' || word[0] > 'Z') && (word[i] < '0' || word[i] > '9'))
            return -1;
    }
    return VAR_NAME;
}

void tokeniseSourcecode( char* sourceCodeFileName, tokenStream *s) {
    FILE* token_fp;
    token_fp = fopen(sourceCodeFileName, "r");
    if(token_fp == NULL) {
        printf("ERROR : SOURCE CODE NOT FOUND\n");
        return;
    }
    char line[200];
    int line_num = 1;
    while(fgets(line, 200, token_fp) != NULL) {
        int len = strlen(line);
        line[len - 1] = '\0';
        
        char *token;
        token = strtok(line, " ");
        
        while (token != NULL) {
            
            struct tokenNode* tokenNode = (struct tokenNode*)malloc(sizeof(struct tokenNode));
            tokenNode -> tokenID = getToken(token);
            tokenNode -> known_symbol = malloc(sizeof(char) * MAX_SIZE);
            strcpy(tokenNode -> known_symbol, token);
            tokenNode -> lineNumber = line_num;
            tokenStream* next_s = (tokenStream*)malloc(sizeof(tokenStream));
            s -> next = next_s;
            next_s -> token = tokenNode;
            next_s -> next = NULL;
            token = strtok(NULL, " ");
        }
        ++line_num;
    }
    printf("Working! %s\n", s->token->known_symbol);
}