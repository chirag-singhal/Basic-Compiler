#include "print.h"

void printParseTree(parseTree* t) {
    if (t == NULL)
        return;
    
    stack st = (stack)malloc(sizeof(struct parserStack));
    stack node = (stack)malloc(sizeof(struct parserStack));
    node -> parseTreeNode = t;
    st = push(node, st);

    while (st != NULL) {
        st 
    }
}

void printTypeExpressionTable(typeExpressionTable T) {
    while(T -> next != NULL) {
        if(T -> row -> typeVariable == PRIMITIVE)
            printf("%20s %d RECTARRAY TYPE = %d ELEMENT TYPE = %d\n", T -> symbol, T -> row -> typeVariable, T -> row -> rectArrayType, T -> row -> expression.primitive);
        else if(T -> row -> typeVariable == RECTARRAY) {
            printf("%20s %d %d DIM = %d RANGES = ", T -> symbol, T -> row -> typeVariable, T -> row -> rectArrayType, T -> row -> expression.rectArray.dimensions);
            for(int i = 0; i < T -> row -> expression.rectArray.dimensions; i++) {
                printf("R%d = ", i + 1);
                for(int j = 0; j < 2; j++) {
                    if(T -> row -> expression.rectArray.ranges[i][j].isVarId)
                        printf("%20s ", T -> row -> expression.rectArray.ranges[i][j].value.varIdIndex);
                    else
                        printf("%d ", T -> row -> expression.rectArray.ranges[i][j].value.intIndex);
                }
            }
            printf("ELEMENT TYPE = %d\n", T -> row -> expression.rectArray.basicElementType);
        }
        else {
            printf("%20s %d %d DIM = %d R1 = %d %d R2 = ", T -> symbol, T -> row -> typeVariable, T -> row -> rectArrayType, T -> row -> expression.jaggedArray.dimensions, T -> row -> expression.jaggedArray.R1_range[0], T -> row -> expression.jaggedArray.R1_range[1]);
            if(T -> row -> expression.jaggedArray.dimensions == 2) {
                for(int i = 0; i < T -> row -> expression.jaggedArray.R1_range[1] - T -> row -> expression.jaggedArray.R1_range[0] + 1; i++)
                    printf("%d ", T -> row -> expression.jaggedArray.R2_ranges.range_2d[i]);
            }
            else {
                for(int i = 0; i < T -> row -> expression.jaggedArray.R1_range[1] - T -> row -> expression.jaggedArray.R1_range[0] + 1; i++) {
                    printf("%d | ", T -> row -> expression.jaggedArray.R2_ranges.range_3d[i][0]);
                    for(int j = 1; j < T -> row -> expression.jaggedArray.R2_ranges.range_3d[i][0]; j++) {
                        printf("%d ", T -> row -> expression.jaggedArray.R2_ranges.range_3d[i][j]);
                    }
                    printf("| ");
                }
            }
            printf("ELEMENT TYPE = %d\n", T -> row -> expression.jaggedArray.basicElementType);
        }
    }
}