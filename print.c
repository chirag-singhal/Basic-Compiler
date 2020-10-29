/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/

#include "print.h"


void printTypeExpression(typeExpressionRow* row) {
    if(row == NULL)
        printf("***");
    else if(row -> typeVariable == PRIMITIVE)
        printf("%d (RECTARRAY TYPE = %d) (ELEMENT TYPE = %d)", row -> typeVariable, row -> rectArrayType, row -> expression.primitive);
    else if(row -> typeVariable == RECTARRAY) {
        printf("%d %d (DIM = %d) (RANGES = ", row -> typeVariable, row -> rectArrayType, row -> expression.rectArray.dimensions);
        for(int i = 0; i < row -> expression.rectArray.dimensions; i++) {
            printf("R%d = ", i + 1);
            for(int j = 0; j < 2; j++) {
                if(row -> expression.rectArray.ranges[i][j].isVarId)
                    printf("%20s ",row -> expression.rectArray.ranges[i][j].value.varIdIndex);
                else
                    printf("%d ", row -> expression.rectArray.ranges[i][j].value.intIndex);
            }
        }
        printf(") (ELEMENT TYPE = %d)", row -> expression.rectArray.basicElementType);
    }
    else {
        printf("%d %d (DIM = %d) (R1 = %d %d) (R2 = ", row -> typeVariable, row -> rectArrayType, row -> expression.jaggedArray.dimensions, row -> expression.jaggedArray.R1_range[0], row -> expression.jaggedArray.R1_range[1]);
        if(row -> expression.jaggedArray.dimensions == 2) {
            for(int i = 0; i < row -> expression.jaggedArray.R1_range[1] - row -> expression.jaggedArray.R1_range[0] + 1; i++)
                printf("%d ", row -> expression.jaggedArray.R2_ranges.range_2d[i]);
        }
        else {
            for(int i = 0; i < row -> expression.jaggedArray.R1_range[1] - row -> expression.jaggedArray.R1_range[0] + 1; i++) {
                printf("%d | ", row -> expression.jaggedArray.R2_ranges.range_3d[i][0]);
                for(int j = 1; j < row -> expression.jaggedArray.R2_ranges.range_3d[i][0]; j++) {
                    printf("%d ", row -> expression.jaggedArray.R2_ranges.range_3d[i][j]);
                }
                printf("| ");
            }
        }
        printf(") (ELEMENT TYPE = %d)", row -> expression.jaggedArray.basicElementType);
    }
}

void printParseTree(parseTree* t) {
    if (t == NULL)
        return;

    stack st = (stack)malloc(sizeof(struct parserStack));
    st -> parseTreeNode = t;
    st -> next = NULL;
    while (st != NULL) {
        // print st (top of stack)
        printf("SYMBOL = %40s | TERMINAL = %d | ", st -> parseTreeNode -> token -> symbol, st -> parseTreeNode -> terminal);
        if (st -> parseTreeNode -> terminal) {
            printf(" LEXEME = %20s | LINENUM = %d ", st -> parseTreeNode -> sourceToken, st -> parseTreeNode -> token -> lineNumber);
        }
        else {
            printf(" TYPEEXPR = ");
            printTypeExpression(st -> parseTreeNode -> typeExpression);
            printf(" | GRAMMARRULE = %d ", st -> parseTreeNode -> grammar_rule_idx);
        }
        printf(" | DEPTH = %d\n", st -> parseTreeNode -> depth);
        //
        stack temp = st;
        st = pop(st);
        // getchar();
        // add children from right to left
        for (int i = temp -> parseTreeNode -> num_child-1; i >= 0; --i) {
            stack temp2 = (stack)malloc(sizeof(struct parserStack));
            temp2 -> parseTreeNode = temp -> parseTreeNode -> children[i];
            temp2 -> next = NULL;
            st = push(temp2, st);
        }
        //
        free(temp);
    }
}

void printTypeExpressionTable(typeExpressionTable T) {
    while(T -> next != NULL) {
        printf("\n%20s ", T -> symbol);
        printTypeExpression(T -> row);
        T = T -> next;
    }
}