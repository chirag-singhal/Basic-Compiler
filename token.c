/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/


#include "token.h"
#include "grammar.h"

#define MAX_SIZE 50


void tokeniseSourcecode( char* sourceCodeFileName, tokenStream *s) {
    FILE* token_fp;
    token_fp = fopen(sourceCodeFileName, "r");
    if(token_fp == NULL) {
        printf("ERROR : SOURCE CODE NOT FOUND\n");
        return;
    }
    int line_num = 1;
    char line[200];
    while(fgets(line, 200, token_fp)) {
        int len = strlen(line);
        
        char *token;
        token = strtok(line, " \t\n");
        
        while (token != NULL) {
            if(s -> token == NULL) {
                struct tokenNode* tokenNode = (struct tokenNode*)malloc(sizeof(struct tokenNode));                
                tokenNode -> tokenID = getToken(token);
                tokenNode -> symbol = malloc(sizeof(char) * MAX_SIZE);
                strcpy(tokenNode -> symbol, token);
                tokenNode -> lineNumber = line_num;
                s -> token = tokenNode;
                s -> next = NULL;
                token = strtok(NULL, " \t\n");
            }
            else {
                struct tokenNode* tokenNode = (struct tokenNode*)malloc(sizeof(struct tokenNode));
                
                tokenNode -> tokenID = getToken(token);
                tokenNode -> symbol = malloc(sizeof(char) * MAX_SIZE);
                strcpy(tokenNode -> symbol, token);
                tokenNode -> lineNumber = line_num;
                tokenStream* next_s = (tokenStream*)malloc(sizeof(tokenStream));
                s -> next = next_s;
                next_s -> token = tokenNode;
                next_s -> next = NULL;
                s = next_s;
                token = strtok(NULL, " \t\n");
            }
        }
        ++line_num;
    }
    fclose(token_fp);
}