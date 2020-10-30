/*
    Group Number: 56
    Chirag Singhal 2018A7PS0219P
    Harshan Baskar 2018A7PS0166P
    Nitya Mangal   2018A7PS0216P
*/


#include "typeExpression.h"

#define MAX_SIZE 50


char* getLexemesFromType(typeVariableEnum typeVariable) {
    if (typeVariable == PRIMITIVE)
        return "PRIMITIVE";
    else if (typeVariable == RECTARRAY)
        return "RECTARRAY";
    else if (typeVariable == JAGGEDARRAY)
        return "JAGGEDARRAY";
    else
        return "ERROR";
}

char* getLexemeFromElementType(basicElementTypeEnum val) {
    if (val == INTEGER_TYPE)
        return "INTEGER";
    else if (val == REAL_TYPE)
        return "REAL";
    else
        return "BOOLEAN";
}

typeExpressionRow* search_typeExpTable(char* symbol, typeExpressionTable T) {
    while (T != NULL) {
        if (strcmp(T -> symbol, symbol) == 0)
            return T -> row;
        T = T -> next;
    }
    // variable does not exist OR table is empty
    return NULL;
}

void push_typeExpTable(char* symbol, typeExpressionRow* row, typeExpressionTable T) {
    //if (T -> symbol == NULL) {
    if (T == NULL) {
        T = (typeExpressionTable)malloc(sizeof(typeExpressionTableNode));
        // T -> symbol = malloc(sizeof(char) * MAX_SIZE);
        // strcpy(T -> symbol, symbol);
        T -> symbol = symbol;
        T -> row = row;
        T -> next = NULL;
        T -> prev = NULL;
        return;
    }
    else if (T -> symbol == NULL) {
        // T -> symbol = malloc(sizeof(char) * MAX_SIZE);
        // strcpy(T -> symbol, symbol);
        T -> symbol = symbol;
        T -> row = row;
        T -> next = NULL;
        T -> prev = NULL;
        return;
    }

    while (T -> next != NULL) {
        T = T -> next;
    }
    T -> next = (typeExpressionTable)malloc(sizeof(typeExpressionTableNode));
    T -> next -> next = NULL;
    T -> next -> prev = T;
    T = T -> next;
    // T -> symbol = malloc(sizeof(char) * MAX_SIZE);
    // strcpy(T -> symbol, symbol);
    T -> symbol = symbol;
    T -> row = row;
}

void printTerm(parseNode* t) {
    if(t -> children[0] -> terminal)
        printf("%10s ", t -> children[0] -> sourceToken);
    else {
        printf("%10s ", t -> children[0] -> children[0] -> sourceToken);
        parseNode* temp = t -> children[0] -> children[1]; //element Dim Optional
        if(temp -> num_child == 1)
            return;
        printf("%s ", temp -> children[0] -> sourceToken);
        temp = temp -> children[1];
        while(temp -> num_child == 2) {
            printf("%s ", temp -> children[0] -> children[0] -> sourceToken);
            temp = temp -> children[1];
        }
        printf("%s ", t -> children[0] -> children[1] -> children[2] -> sourceToken);
    }
}

void printMul(parseNode* t) {
    printTerm(t -> children[0]);
}

void printAdd(parseNode* t) {
    printMul(t -> children[0]);
}

void printAnd(parseNode* t) {
    printAdd(t -> children[0]);
}


void handleDeclarativeStmt(parseNode* t, typeExpressionTable T) {
    /*
        if varOrListDeclare, check first child
        if VAR_ID, add only 1 row
        else add multiple 
    */
    // t -> children[1] Specific Declarative Phrase
    t -> token -> lineNumber = t -> children[0] -> children[0] -> token -> lineNumber;
    if(t -> children[1] -> children[0] -> token -> tokenID == primitiveDeclarativePhrase) {
        //Primitive
        t -> children[1] -> typeExpression -> typeVariable = PRIMITIVE;
        t -> children[1] -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        if(t -> children[1] -> children[0] -> children[0] -> children[0] -> token -> tokenID == INTEGER) {
            //INTEGER
            (t -> children[1] -> typeExpression -> expression).primitive = INTEGER_TYPE;
        }
        else if (t -> children[1] -> children[0] -> children[0] -> children[0] -> token -> tokenID == BOOLEAN) {
            //BOOLEAN
            (t -> children[1] -> typeExpression -> expression).primitive = BOOLEAN_TYPE;
        }
        else {
            //REAL
            (t -> children[1] -> typeExpression -> expression).primitive = REAL_TYPE;
        }
        
        // push variable to type expression table
        if (t -> children[0] -> children[1] -> num_child == 1) {
            // t -> children[0] -> children[1] -> children[0] is the parse node pointing to the VAR_ID
            t -> token -> lineNumber = t -> children[0] -> children[1] -> children[0] -> token -> lineNumber;
            push_typeExpTable(t -> children[0] -> children[1] -> children[0] -> sourceToken, t -> children[1] -> typeExpression, T);
        }
        else {
            // t -> children[0] -> children[1] -> children[3] is the parse node pointing to listVarId
            parseNode* temp = t -> children[0] -> children[1] -> children[3];
            t -> token -> lineNumber = temp -> children[0] -> token -> lineNumber;
            while (temp -> num_child == 2) {
                push_typeExpTable(temp -> children[0] -> sourceToken, t -> children[1] -> typeExpression, T);
                temp = temp -> children[1];
            }
        }
    }
    else if(t -> children[1] -> children[0] -> token -> tokenID == rectArrayDeclarativePhrase) {
        //RectArray INTEGER
        t -> children[1] -> typeExpression -> typeVariable = RECTARRAY;

        int numOfDim = 0;
        enum _typeRectArrayEnum rectArrayType = STATIC;
        struct rectArray rectArr;
        rectArr.ranges = (rectArrayIndex**) malloc(sizeof(rectArrayIndex*) * 32);

        parseNode* temp = t -> children[1] -> children[0] -> children[1]; // listDim
        
        while(temp -> num_child != 1) {
            rectArr.ranges[numOfDim] = malloc(sizeof(rectArrayIndex) * 2);
            // first range index
            if(temp -> children[1] -> children[0] -> token -> tokenID == VAR_ID) {
                // check if VAR_ID is INTEGER from type expression table
                typeExpressionRow* row = search_typeExpTable(temp -> children[1] -> children[0] -> sourceToken, T);
                if (row == NULL || row -> typeVariable != PRIMITIVE || (row -> expression).primitive != INTEGER_TYPE) {
                    // error
                    //lineNumber, statement type, operator, firstOperandLexeme, type,
                    //secondOperandLexeme, type, position in parse tree, message 
                    printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Declaration statement", t->token->symbol, "***", "***", "***", t -> depth, "RA invalid element type");
                }
                rectArr.ranges[numOfDim][0].isVarId = 1;
                rectArr.ranges[numOfDim][0].value.varIdIndex = temp -> children[1] -> children[0] -> sourceToken;
            }
            else {
                rectArr.ranges[numOfDim][0].isVarId = 0;
                rectArr.ranges[numOfDim][0].value.intIndex = atoi(temp -> children[1] -> children[0] -> sourceToken);
            }
            // second range index
            if(temp -> children[3] -> children[0] -> token -> tokenID == VAR_ID) {
                typeExpressionRow* row = search_typeExpTable(temp -> children[3] -> children[0] -> sourceToken, T);
                if (row == NULL || row -> typeVariable != PRIMITIVE || (row -> expression).primitive != INTEGER_TYPE) {
                    // error
                    printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Declaration statement", t->token->symbol, getLexemesFromType(row->typeVariable), "***", "***", t -> depth, "RA type mismatch");
                }
                rectArr.ranges[numOfDim][1].isVarId = 1;
                rectArr.ranges[numOfDim][1].value.varIdIndex = temp -> children[3] -> children[0] -> sourceToken;
            }
            else {
                rectArr.ranges[numOfDim][1].isVarId = 0;
                rectArr.ranges[numOfDim][1].value.intIndex = atoi(temp -> children[3] -> children[0] -> sourceToken);
            }

            if(temp -> children[1] -> children[0] -> token -> tokenID == VAR_ID || temp -> children[3] -> children[0] -> token -> tokenID == VAR_ID)
                rectArrayType = DYNAMIC;
            numOfDim++;
            temp = temp -> children[5];
        }
        t -> children[1] -> typeExpression -> rectArrayType = rectArrayType;
        rectArr.dimensions = numOfDim;
        rectArr.basicElementType = INTEGER_TYPE;
        (t -> children[1] -> typeExpression -> expression).rectArray = rectArr;
        
        // push variable to type expression table
        if (t -> children[0] -> children[1] -> num_child == 1) {
            // t -> children[0] -> children[1] -> children[0] is the parse node pointing to the VAR_ID
            t -> token -> lineNumber = t -> children[0] -> children[1] -> children[0] -> token -> lineNumber;
            push_typeExpTable(t -> children[0] -> children[1] -> children[0] -> sourceToken, t -> children[1] -> typeExpression, T);
        }
        else {
            // t -> children[0] -> children[1] -> children[3] is the parse node pointing to listVarId
            parseNode* temp = t -> children[0] -> children[1] -> children[3];
            t -> token -> lineNumber = temp -> children[0] -> token -> lineNumber;
            while (temp -> num_child == 2) {
                push_typeExpTable(temp -> children[0] -> sourceToken, t -> children[1] -> typeExpression, T);
                temp = temp -> children[1];
            }
        }
    }
    else{
        //jaggedArray INTEGER
        t -> children[1] -> typeExpression -> typeVariable = JAGGEDARRAY;
        t -> children[1] -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        int numDim;
        // temp is the pointer to listEmptyDim
        parseNode* temp = t -> children[1] -> children[0] -> children[7];
        if(temp -> children[2] -> num_child == 1)
            numDim = 2;
        else // if(temp -> children[2] -> num_child == 3)
            numDim = 3;

        struct jaggedArray jaggedArray;
        jaggedArray.dimensions = numDim;
        if (t -> children[1] -> children[0] -> children[3] -> token -> tokenID == VAR_ID || t -> children[1] -> children[0] -> children[5] -> token -> tokenID == VAR_ID) {
            // error
            printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Declaration statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "JA invalid range");
        }
        jaggedArray.R1_range[0] = atoi(t -> children[1] -> children[0] -> children[3] -> sourceToken);
        jaggedArray.R1_range[1] = atoi(t -> children[1] -> children[0] -> children[5] -> sourceToken);
        
        if (numDim == 2)
            jaggedArray.R2_ranges.range_2d = (int*)malloc((jaggedArray.R1_range[1]-jaggedArray.R1_range[0]+1) * sizeof(int));
        else
            jaggedArray.R2_ranges.range_3d = (int**)malloc((jaggedArray.R1_range[1]-jaggedArray.R1_range[0]+1) * sizeof(int*));
        
        jaggedArray.basicElementType = INTEGER_TYPE;
        // we need to count for third dimension
        // now, temp is the pointer to jaggedArrayInitialization
        temp = t -> children[1] -> children[1];

        while (temp -> num_child == 2) {
            temp -> children[0] -> children[2]; // R1 : NUM
            temp -> children[0] -> children[6]; // SIZE : NUM
            temp -> children[0] -> children[10]; // pointer to listIntSemiColon
            // do something
            if (numDim == 2)
                jaggedArray.R2_ranges.range_2d[atoi(temp -> children[0] -> children[2] -> sourceToken)-jaggedArray.R1_range[0]] = atoi(temp -> children[0] -> children[6] -> sourceToken);
            else {
                jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> sourceToken)-jaggedArray.R1_range[0]] = (int*)malloc((atoi(temp -> children[0] -> children[6] -> sourceToken)+1) * sizeof(int));
            }
            
            parseNode* temp2 = temp -> children[0] -> children[10];
            // temp2 points to listIntSemiColon
            if (numDim == 3)
                jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> sourceToken)-jaggedArray.R1_range[0]][0] = atoi(temp -> children[0] -> children[6] -> sourceToken);
            int count = 0;
            while (temp2 -> num_child == 3 || temp2 -> num_child == 2) {
                if (numDim == 2 && count >= jaggedArray.R2_ranges.range_2d[atoi(temp -> children[0] -> children[2] -> sourceToken)-jaggedArray.R1_range[0]]) {
                    // error
                    printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Declaration statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "2D JA excess elements");
                }
                if (numDim == 3 && count >= jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> sourceToken)-jaggedArray.R1_range[0]][0]) {
                    // error
                    printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Declaration statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "3D JA excess elements");
                }
                // temp3 points to listInt
                parseNode* temp3 = temp2 -> children[temp2 -> num_child-2];
                int count2 = 0;
                while (temp3 -> num_child == 2) {
                    count2++;
                    temp3 = temp3 -> children[1];
                }
                
                if (count2 > 1 && numDim == 2) {
                    // error
                    printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Declaration statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "2D JA excess elements");
                }
                else if (numDim == 3) {
                    jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> sourceToken)-jaggedArray.R1_range[0]][count+1] = count2;
                }
                count++;
                temp2 = temp2 -> children[temp2 -> num_child-1];
            }
            if (numDim == 2 && count < jaggedArray.R2_ranges.range_2d[atoi(temp -> children[0] -> children[2] -> sourceToken)-jaggedArray.R1_range[0]]) {
                // error
                printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Declaration statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "2D JA too few elements");
            }
            if (numDim == 3 && count < jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> sourceToken)-jaggedArray.R1_range[0]][0]) {
                // error
                printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Declaration statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "3D JA too few elements");
            }
            //
            temp = temp -> children[1];
        }
        (t -> children[1] -> typeExpression -> expression).jaggedArray = jaggedArray;
        
        // push variable to type expression table
        if (t -> children[0] -> children[1] -> num_child == 1) {
            // t -> children[0] -> children[1] -> children[0] is the parse node pointing to the VAR_ID
            t -> token -> lineNumber = t -> children[0] -> children[1] -> children[0] -> token -> lineNumber;
            push_typeExpTable(t -> children[0] -> children[1] -> children[0] -> sourceToken, t -> children[1] -> typeExpression, T);
        }
        else {
            // t -> children[0] -> children[1] -> children[3] is the parse node pointing to listVarId
            parseNode* temp = t -> children[0] -> children[1] -> children[3];
            t -> token -> lineNumber = temp -> children[0] -> token -> lineNumber;
            while (temp -> num_child == 2) {
                push_typeExpTable(temp -> children[0] -> sourceToken, t -> children[1] -> typeExpression, T);
                temp = temp -> children[1];
            }
        }
    }
}

int compTypeExp(typeExpressionRow* r1, typeExpressionRow* r2) {
    if(r1 -> typeVariable != r2 -> typeVariable)
        return 0;
    else if(r1 -> typeVariable == RECTARRAY && r1 -> rectArrayType != r2 -> rectArrayType)
        return 0;
    else {
        //primitive
        if(r1 -> typeVariable == PRIMITIVE) {
            if((r1 -> expression).primitive != (r2 -> expression).primitive) {
                return 0;
            }
        }
        //rectangular
        else if(r1 -> typeVariable == RECTARRAY) {
            int dim = (r1->expression).rectArray.dimensions;
            if((r1->expression).rectArray.dimensions != (r2->expression).rectArray.dimensions)
                return 0;
            else if((r1->expression).rectArray.basicElementType != (r1->expression).rectArray.basicElementType)
                return 0;

            for(int i=0; i<dim; i++){
                //low...high
                //for low
                if((r1->expression).rectArray.ranges[i][0].isVarId != (r2->expression).rectArray.ranges[i][0].isVarId)
                    return 0;
                else if((r1->expression).rectArray.ranges[i][1].isVarId != (r2->expression).rectArray.ranges[i][1].isVarId)
                    return 0;
                int v1 = (r1->expression).rectArray.ranges[i][0].isVarId;
                int v2 = (r1->expression).rectArray.ranges[i][1].isVarId;
                            
                if(v1 == 0) {
                    if((r1->expression).rectArray.ranges[i][0].value.intIndex != (r2->expression).rectArray.ranges[i][0].value.intIndex)
                        return 0;
                }
                else if(v1 == 1) {
                    if(!strcmp((r1->expression).rectArray.ranges[i][0].value.varIdIndex, (r2->expression).rectArray.ranges[i][0].value.varIdIndex))
                        return 0;
                }
                //for high
                if(v1 == 0) {
                    if((r1->expression).rectArray.ranges[i][1].value.intIndex != (r2->expression).rectArray.ranges[i][1].value.intIndex)
                        return 0;
                }
                else if(v1 == 1) {
                    if(!strcmp((r1->expression).rectArray.ranges[i][1].value.varIdIndex, (r2->expression).rectArray.ranges[i][1].value.varIdIndex))
                        return 0;
                }
            }
        }
        //jagged
        else if(r1 -> typeVariable == JAGGEDARRAY) {
            int dim = (r1->expression).jaggedArray.dimensions;
            if((r1->expression).jaggedArray.dimensions != (r2->expression).jaggedArray.dimensions)
                return 0;
            else if((r1->expression).jaggedArray.basicElementType != (r1->expression).jaggedArray.basicElementType)  //not required
                return 0;
            else if((r1->expression).jaggedArray.R1_range[0]!=(r2->expression).jaggedArray.R1_range[0] || (r1->expression).jaggedArray.R1_range[1]!=(r2->expression).jaggedArray.R1_range[1])
                return 0;
            if(dim == 2) {
                int num = (r1->expression).jaggedArray.R1_range[1] - (r1->expression).jaggedArray.R1_range[0];
                for(int i=0; i<num; i++) {
                    if((r1->expression).jaggedArray.R2_ranges.range_2d[i] != (r2->expression).jaggedArray.R2_ranges.range_2d[i])
                        return 0;
                }
            }
            else if(dim == 3) {
                int dim2 = (r1->expression).jaggedArray.R1_range[1] - (r1->expression).jaggedArray.R1_range[0];
                for(int i = 0; i < dim2; i++) {
                    int size1 = (r1->expression).jaggedArray.R2_ranges.range_3d[i][0];
                    int size2 = (r2->expression).jaggedArray.R2_ranges.range_3d[i][0];
                    if(size1 != size2)
                        return 0;
                    for(int j=1; j<size1; j++) {
                        if((r1->expression).jaggedArray.R2_ranges.range_3d[i][j] != (r2->expression).jaggedArray.R2_ranges.range_3d[i][j])
                            return 0;
                    }
                }
            }
        }
    }
    return 1;
}
    
void handleElement(parseNode* t, typeExpressionTable T) {
    t -> token -> lineNumber = t -> children[0] -> token -> lineNumber;

    //t = element
    typeExpressionRow* row = search_typeExpTable(t -> children[0] -> sourceToken, T);  //VAR_ID
    
    // if any error, set this new_row to be ERROR and return
    typeExpressionRow* new_row = (typeExpressionRow*)malloc(sizeof(typeExpressionRow));
    
    // this is to set new_row as something which is not ERROR deterministically. INTEGER here does not matter.
    //CHECK THIS LATER DANGER
    new_row -> typeVariable = PRIMITIVE;

    if (row == NULL) {
        // error - variable does not exist
        //lineNumber, statement type, operator, firstOperandLexeme, type,
        //secondOperandLexeme, type, position in parse tree, message
        printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t -> children[0] -> sourceToken, "INVALID", "***", "***", t -> depth, "Variable does not exist");
        new_row -> typeVariable = ERROR;
        new_row -> rectArrayType = NOT_APPLICABLE;
        (new_row -> expression).primitive = INTEGER;
        t -> typeExpression = new_row;
    }
    else if (t -> children[1] -> num_child == 1) {
        // single variable
        t -> typeExpression = row;
    }
    else {
        // Array of type INTEGER
        // temp points to listIdx
        parseNode* temp = t -> children[1] -> children[1];
        int dim = 0;

        if (row -> typeVariable == RECTARRAY) {
            while (temp -> num_child == 2) {
                // check idx with ArrayBounds[dim]
                if (dim >= (row -> expression).rectArray.dimensions) {
                    // error
                    printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "RECTARRAY", "***", "***", t -> depth, "RA index out of bounds");
                    new_row -> typeVariable = ERROR;
                    new_row -> rectArrayType = NOT_APPLICABLE;
                    (new_row -> expression).primitive = INTEGER_TYPE;
                    t -> typeExpression = new_row;
                }
                
                if (temp -> children[0] -> children[0] -> token -> tokenID == NUM) {
                    int idx = atoi(temp -> children[0] -> children[0] -> sourceToken);
                    if ((row -> expression).rectArray.ranges[dim][0].isVarId && search_typeExpTable((row -> expression).rectArray.ranges[dim][0].value.varIdIndex, T) == NULL) {
                        // error - set type expression to ERROR
                        printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "RECTARRAY", "***", "***", t -> depth, "Variable does not exist");
                        new_row -> typeVariable = ERROR;
                        new_row -> rectArrayType = NOT_APPLICABLE;
                        (new_row -> expression).primitive = INTEGER_TYPE;
                        t -> typeExpression = new_row;
                    }
                    else if (!(row -> expression).rectArray.ranges[dim][0].isVarId && (row -> expression).rectArray.ranges[dim][0].value.intIndex > idx) {
                        // error
                        printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "RECTARRAY", "***", "***", t -> depth, "RA index out of bounds");
                        new_row -> typeVariable = ERROR;
                        new_row -> rectArrayType = NOT_APPLICABLE;
                        (new_row -> expression).primitive = INTEGER_TYPE;
                        t -> typeExpression = new_row;
                    }
                    else if ((row -> expression).rectArray.ranges[dim][1].isVarId && (search_typeExpTable((row -> expression).rectArray.ranges[dim][1].value.varIdIndex, T) == NULL)) {
                        // error
                        printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "RECTARRAY", "***", "***", t -> depth, "RA undefined index");
                        new_row -> typeVariable = ERROR;
                        new_row -> rectArrayType = NOT_APPLICABLE;
                        (new_row -> expression).primitive = INTEGER_TYPE;
                        t -> typeExpression = new_row;
                    }
                    else if (!(row -> expression).rectArray.ranges[dim][1].isVarId && (row -> expression).rectArray.ranges[dim][1].value.intIndex < idx) {
                        // error
                        printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "RECTARRAY", "***", "***", t -> depth, "RA index out of bounds");
                        new_row -> typeVariable = ERROR;
                        new_row -> rectArrayType = NOT_APPLICABLE;
                        (new_row -> expression).primitive = INTEGER_TYPE;
                        t -> typeExpression = new_row;
                    }
                }
                else if (search_typeExpTable(temp -> children[0] -> children[0] -> sourceToken, T) == NULL) {
                    // check for valid variable name
                    // error
                    printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "RECTARRAY", "***", "***", t -> depth, "invalid variable name");
                    new_row -> typeVariable = ERROR;
                    new_row -> rectArrayType = NOT_APPLICABLE;
                    (new_row -> expression).primitive = INTEGER_TYPE;
                    t -> typeExpression = new_row;
                }
                dim++;           
                //
                temp = temp -> children[1];
            }
            if (dim < (row -> expression).rectArray.dimensions) {
                // error
                printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "RECTARRAY", "***", "***", t -> depth, "RA index out of bounds");
                new_row -> typeVariable = ERROR;
                new_row -> rectArrayType = NOT_APPLICABLE;
                (new_row -> expression).primitive = INTEGER_TYPE;
                t -> typeExpression = new_row;
            }

            if (new_row -> typeVariable != ERROR) {
                new_row -> typeVariable = PRIMITIVE;
                new_row -> rectArrayType = NOT_APPLICABLE;
                (new_row -> expression).primitive = INTEGER_TYPE;
                t -> typeExpression = new_row;
            }
        }

        //temp points to listIdx
        else if(row -> typeVariable == JAGGEDARRAY) {
            // handle only R1 range
            if (temp -> children[0] -> children[0] -> token -> tokenID == NUM) {
                int idx = atoi(temp -> children[0] -> children[0] -> sourceToken);
                if (idx < (row -> expression).jaggedArray.R1_range[0] || idx > (row -> expression).jaggedArray.R1_range[1]) {
                    // error
                    printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "JA index out of bounds");
                    new_row -> typeVariable = ERROR;
                    new_row -> rectArrayType = NOT_APPLICABLE;
                    (new_row -> expression).primitive = INTEGER_TYPE;
                    t -> typeExpression = new_row;
                }
                temp = temp -> children[1];

                if ((row -> expression).jaggedArray.dimensions == 2) {
                    // 2 dim jagged array
                    if (temp -> children[1] -> num_child == 2) {
                        // error
                        printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "JA invalid dimensions");
                        new_row -> typeVariable = ERROR;
                        new_row -> rectArrayType = NOT_APPLICABLE;
                        (new_row -> expression).primitive = INTEGER_TYPE;
                        t -> typeExpression = new_row;
                    }
                    if (temp -> children[1] -> children[0] -> token -> tokenID == NUM) {
                        int idx2 = atoi(temp -> children[1] -> children[0] -> children[0] -> sourceToken);
                        if(idx2 > (row -> expression).jaggedArray.R2_ranges.range_2d[idx - (row -> expression).jaggedArray.R1_range[0]] - 1) {
                            //error
                            printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "3D JA size mismatch");
                            new_row -> typeVariable = ERROR;
                            new_row -> rectArrayType = NOT_APPLICABLE;
                            (new_row -> expression).primitive = INTEGER_TYPE;
                            t -> typeExpression = new_row;
                        }
                    }
                }
                else {
                    // 3 dim jagged array
                    if (temp -> children[1] -> num_child == 1) {
                        // error
                        printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "3D JA too few dimensions");
                        new_row -> typeVariable = ERROR;
                        new_row -> rectArrayType = NOT_APPLICABLE;
                        (new_row -> expression).primitive = INTEGER_TYPE;
                        t -> typeExpression = new_row;
                    }
                    //R2 range
                    if (temp -> children[1] -> children[0] -> children[0] -> token -> tokenID == NUM) {
                        int idx2 = atoi(temp -> children[1] -> children[0] -> children[0] -> sourceToken);
                        if(idx2 > (row -> expression).jaggedArray.R2_ranges.range_3d[idx - (row -> expression).jaggedArray.R1_range[0]][0] - 1) {
                            //error
                            printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "3D JA index out of bounds");
                            new_row -> typeVariable = ERROR;
                            new_row -> rectArrayType = NOT_APPLICABLE;
                            (new_row -> expression).primitive = INTEGER_TYPE;
                            t -> typeExpression = new_row;
                        }
                    
                        //R3 range
                        if (temp -> children[1] -> children[1] -> children[0] -> children[0] -> token -> tokenID) {
                            int idx3 = atoi(temp -> children[1] -> children[1] -> children[0] -> children[0] -> sourceToken);
                            if(idx3 > (row -> expression).jaggedArray.R2_ranges.range_3d[idx - (row -> expression).jaggedArray.R1_range[0]][idx2] - 1) {
                                //error
                                printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "JAGARRAY", "***", "***", t -> depth, "3D JA index out of bounds");
                                new_row -> typeVariable = ERROR;
                                new_row -> rectArrayType = NOT_APPLICABLE;
                                (new_row -> expression).primitive = INTEGER_TYPE;
                                t -> typeExpression = new_row;
                            }
                        }
                    }
                    else if (search_typeExpTable(temp -> children[1] -> children[0] -> children[0] -> sourceToken, T) == NULL) {
                        // check for valid variable name
                        // error
                        printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "NA", "***", "***", t -> depth, "invalid variable name");
                        new_row -> typeVariable = ERROR;
                        new_row -> rectArrayType = NOT_APPLICABLE;
                        (new_row -> expression).primitive = INTEGER_TYPE;
                        t -> typeExpression = new_row;
                    }
                }
            }
            else if (search_typeExpTable(temp -> children[0] -> children[0] -> sourceToken, T) == NULL) {
                // check for valid variable name
                // error
                printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "NA", "***", "***", t -> depth, "invalid variable name");
                new_row -> typeVariable = ERROR;
                new_row -> rectArrayType = NOT_APPLICABLE;
                (new_row -> expression).primitive = INTEGER_TYPE;
                t -> typeExpression = new_row;
            }

            if (new_row -> typeVariable != ERROR) {
                new_row -> typeVariable = PRIMITIVE;
                new_row -> rectArrayType = NOT_APPLICABLE;
                (new_row -> expression).primitive = INTEGER_TYPE;
                t -> typeExpression = new_row;
            }
        }
        else {
            //error
            printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, getLexemeFromElementType((t->typeExpression->expression).primitive), "***", "***", t -> depth, "no such element exists");
            new_row -> typeVariable = ERROR;
            new_row -> rectArrayType = NOT_APPLICABLE;
            (new_row -> expression).primitive = INTEGER_TYPE;
            t -> typeExpression = new_row;
        }
    }
}

void handleTerm(parseNode* t, typeExpressionTable T) {
    if (t -> children[0] -> token -> tokenID == NUM) {
        t -> token -> lineNumber = t -> children[0] -> token -> lineNumber;
        // NUM
        typeExpressionRow* row = (typeExpressionRow*)malloc(sizeof(typeExpressionRow));
        row -> typeVariable = PRIMITIVE;
        row -> rectArrayType = NOT_APPLICABLE;
        (row -> expression).primitive = INTEGER_TYPE;
        t -> typeExpression = row;
    }
    else if (t -> children[0] -> token -> tokenID == VAR_ID) {
        // element
        handleElement(t -> children[0], T);
        t -> token -> lineNumber = t -> children[0] -> token -> lineNumber;
        t -> typeExpression = t -> children[0] -> typeExpression;
    }
    else {
        // invalid variable
        // error
        printf("\n\nINVALID VAR NAME\n\n");
    }
}

void handleMulLogicExpr(parseNode* t, typeExpressionTable T) {
    handleTerm(t -> children[0], T);
    t -> token -> lineNumber = t -> children[0] -> token -> lineNumber;
    if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
        t -> typeExpression = t -> children[0] -> typeExpression;
        return;
    }
    stack head = malloc(sizeof(struct parserStack));
    head -> parseTreeNode = t;
    head -> next = NULL;
    parseNode* temp = t -> children[1]; // mulOptional
    while(temp -> num_child == 3) {
        stack node = malloc(sizeof(struct parserStack));
        node -> parseTreeNode = temp;
        node -> next = NULL;
        head = push(node, head);
        temp = temp -> children[2];
    }
    typeExpressionRow* prev = NULL;
    int prevMulOp = -1;
    while(head -> parseTreeNode != t) {
        handleTerm(head -> parseTreeNode -> children[1], T);
        if(prev == NULL) {
            head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
            prev = (typeExpressionRow*)malloc(sizeof(typeExpressionRow));
            prev -> expression = head -> parseTreeNode -> typeExpression -> expression;
            prev -> rectArrayType = head -> parseTreeNode -> typeExpression -> rectArrayType;
            prev -> typeVariable = head -> parseTreeNode -> typeExpression -> typeVariable;
            prevMulOp = head -> parseTreeNode -> children[0] -> children[0] -> token -> tokenID == MUL_OP;
        }
        else {
            if(prev -> typeVariable == ERROR) {
                //Error propagation
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable == ERROR) {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
                prev = head -> parseTreeNode -> children[1] -> typeExpression;
            }
            else if(prev -> typeVariable == PRIMITIVE && prev -> expression.primitive == BOOLEAN_TYPE) {
                //errorsourceToken
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE; 
                head -> parseTreeNode -> typeExpression = prev;
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> children[0] -> sourceToken);
                printTerm(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printTerm(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Boolean NA for operator");
                // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "BOOLEAN", "***", "***", t -> depth, "Booleans can't be multiplied");
            }
            else if(compTypeExp(prev, head -> parseTreeNode -> children[1] -> typeExpression) == 0) {
                //error
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE; 
                head -> parseTreeNode -> typeExpression = prev;
                //set prev as error also
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> children[0] -> sourceToken);
                printTerm(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printTerm(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Type not matching");
                // printf("ERROR: %2d %25s %3s %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "*/", t->token->symbol, prev->typeVariable, head->parseTreeNode -> children[1]->token->symbol, head -> parseTreeNode -> children[1] -> typeExpression->typeVariable, t -> depth, "type mismatch");
            }
            else if (head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable == PRIMITIVE && (head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive == BOOLEAN_TYPE) {
                //error 
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> children[0] -> sourceToken);
                printTerm(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printTerm(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Boolean NA for operator");
                // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, (t->typeExpression->expression).primitive, "***", "***", t -> depth, "Booleans can't be multiplied");
                //set prev as error also
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE; 
                head -> parseTreeNode -> typeExpression = prev;
            }
            else  {
                if(prevMulOp) {
                    head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
                    prev -> expression = head -> parseTreeNode -> typeExpression -> expression;
                    prev -> rectArrayType = head -> parseTreeNode -> typeExpression -> rectArrayType;
                    prev -> typeVariable = head -> parseTreeNode -> typeExpression -> typeVariable;
                }
                else {
                    typeExpressionRow* typeExpression = malloc(sizeof(typeExpressionRow));
                    typeExpression -> expression = head -> parseTreeNode -> children[1] -> typeExpression -> expression;
                    typeExpression -> rectArrayType = head -> parseTreeNode -> children[1] -> typeExpression -> rectArrayType;
                    typeExpression -> typeVariable = head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable;
                    if(typeExpression -> typeVariable == PRIMITIVE)
                        typeExpression -> expression.primitive = REAL_TYPE;
                    else if(typeExpression -> typeVariable == RECTARRAY)
                        typeExpression -> expression.rectArray.basicElementType = REAL_TYPE;
                    else 
                        typeExpression -> expression.jaggedArray.basicElementType = REAL_TYPE;
                    head -> parseTreeNode -> typeExpression = typeExpression;
                    prev = typeExpression;
                }
                prevMulOp = head -> parseTreeNode -> children[0] -> children[0] -> token -> tokenID == MUL_OP;
            }
        }
        stack top = head;
        head = pop(head);
        free(top);
    }
    if(prev -> typeVariable == ERROR) {
        //Error propagation
        head -> parseTreeNode -> typeExpression = prev;
    }
    else if(prev -> typeVariable == PRIMITIVE && prev -> expression.primitive == BOOLEAN_TYPE) {
        //errorsourceToken
        t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] ->  children[0] -> sourceToken);
        printTerm(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printTerm(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Boolean NA for operator");
        // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, "BOOLEAN", "***", "***", t -> depth, "Booleans can't be multiplied");
    }
    else if(compTypeExp(prev, head -> parseTreeNode -> children[0] -> typeExpression) == 0) {
        //error
        t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        //set prev as error also
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] -> children[0] -> sourceToken);
        printTerm(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printTerm(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Type not matching");
        // printf("ERROR: %2d %25s %3s %20s %10s %20s %2d %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "*/", t->token->symbol, prev->typeVariable, head->parseTreeNode -> children[1]->token->symbol, head -> parseTreeNode -> children[1] -> typeExpression->typeVariable, t -> depth, "type mismatch");
    }
    else if (head -> parseTreeNode -> children[0] -> typeExpression -> typeVariable == PRIMITIVE && (head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive == BOOLEAN_TYPE) {
        //error 
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] ->  children[0] -> sourceToken);
        printTerm(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printTerm(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Boolean NA for operator");
        // printf("ERROR: %2d %25s *** %20s %10s %20s %2d %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, (t->typeExpression->expression).primitive, "***", "***", t -> depth, "Booleans can't be multiplied");
        t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
    }
    else  {
        if(prevMulOp) {
            head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
            prev -> expression = head -> parseTreeNode -> typeExpression -> expression;
            prev -> rectArrayType = head -> parseTreeNode -> typeExpression -> rectArrayType;
            prev -> typeVariable = head -> parseTreeNode -> typeExpression -> typeVariable;
        }
        else {
            typeExpressionRow* typeExpression = malloc(sizeof(typeExpressionRow));
            typeExpression -> expression = head -> parseTreeNode -> children[1] -> typeExpression -> expression;
            typeExpression -> rectArrayType = head -> parseTreeNode -> children[1] -> typeExpression -> rectArrayType;
            typeExpression -> typeVariable = head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable;
            if(typeExpression -> typeVariable == PRIMITIVE)
                typeExpression -> expression.primitive = REAL_TYPE;
            else if(typeExpression -> typeVariable == RECTARRAY)
                typeExpression -> expression.rectArray.basicElementType = REAL_TYPE;
            else 
                typeExpression -> expression.jaggedArray.basicElementType = REAL_TYPE;
            head -> parseTreeNode -> typeExpression = typeExpression;
            prev = typeExpression;
        }
    }
    stack top = head;
    head = pop(head);
    free(top);
}

void handleAddLogicExpr(parseNode* t, typeExpressionTable T) {
    handleMulLogicExpr(t -> children[0], T);
    t -> token -> lineNumber = t -> children[0] -> token -> lineNumber;
    if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
        t -> typeExpression = t -> children[0] -> typeExpression;
        return;
    }
    stack head = malloc(sizeof(struct parserStack));
    head -> parseTreeNode = t;
    head -> next = NULL;
    parseNode* temp = t -> children[1]; // addOptional
    while(temp -> num_child == 3) {
        stack node = malloc(sizeof(struct parserStack));
        node -> parseTreeNode = temp;
        node -> next = NULL;
        head = push(node, head);
        temp = temp -> children[2];
    }

    typeExpressionRow* prev = NULL;
    while(head -> parseTreeNode != t) {
        handleMulLogicExpr(head -> parseTreeNode -> children[1], T);
        if(prev == NULL) {
            head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
            prev = (typeExpressionRow*)malloc(sizeof(typeExpressionRow));
            prev -> expression = head -> parseTreeNode -> typeExpression -> expression;
            prev -> rectArrayType = head -> parseTreeNode -> typeExpression -> rectArrayType;
            prev -> typeVariable = head -> parseTreeNode -> typeExpression -> typeVariable;
        }
        else {
            if(prev -> typeVariable == ERROR ) {
                //Error propagation
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable == ERROR) {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
                prev = head -> parseTreeNode -> children[1] -> typeExpression;
            }
            else if(prev -> typeVariable == PRIMITIVE && prev -> expression.primitive == BOOLEAN_TYPE) {
                //error
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> children[0] -> sourceToken);
                printMul(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printMul(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Boolean NA for operator");
                // printf("ERROR: %2d %25s %3s %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "|||", t->token->symbol, prev->expression.primitive, "***", "***", t -> depth, "invalid type for operator");
                //set prev as error
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE;
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(compTypeExp(prev, head -> parseTreeNode -> children[1] -> typeExpression) == 0) {
                //error
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> children[0] -> sourceToken);
                printMul(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printMul(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Type not matching");
                // printf("ERROR: %2d %25s %3s %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "|||", t->token->symbol, prev->expression.primitive, "***", "***", t -> depth, "invalid type for operator");
                //set prev as error
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE; 
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if (head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable == PRIMITIVE && (head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive == BOOLEAN_TYPE) {
                //error 
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> children[0] -> sourceToken);
                printMul(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printMul(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Boolean NA for operator");
                // printf("ERROR: %2d %25s %3s %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "|||", t->token->symbol, prev->expression.primitive, "***", "***", t -> depth, "invalid type for operator");
                //set prev as error
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE; 
                head -> parseTreeNode -> typeExpression = prev;
            }
            else  {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
                prev -> expression = head -> parseTreeNode -> typeExpression -> expression;
                prev -> rectArrayType = head -> parseTreeNode -> typeExpression -> rectArrayType;
                prev -> typeVariable = head -> parseTreeNode -> typeExpression -> typeVariable;
            }
        }
        stack top = head;
        head = pop(head);
        free(top);
    }
    if(prev -> typeVariable == ERROR) {
        head -> parseTreeNode -> typeExpression = prev;
    }
    else if(prev -> typeVariable == PRIMITIVE && prev -> expression.primitive == BOOLEAN_TYPE) {
        //error
        // t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] -> children[0] -> sourceToken);
        printMul(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printMul(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Boolean NA for operator");
        // printf("ERROR: %2d %25s %3s %20s %2d %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "|||", t->token->symbol, prev->expression.primitive, "***", "***", t -> depth, "invalid type for operator");
    }
    else if(compTypeExp(prev, head -> parseTreeNode -> children[0] -> typeExpression) == 0) {
        //error
        // t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] -> children[0] -> sourceToken);
        printMul(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printMul(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Type not matching");
        // printf("ERROR: %2d %25s %3s %20s %2d %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "|||", t->token->symbol, prev->expression.primitive, "***", "***", t -> depth, "type mismatch error");
    }
    else if(head -> parseTreeNode -> children[0] -> typeExpression -> typeVariable == PRIMITIVE && (head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive == BOOLEAN_TYPE) {
        //error
        // t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] -> children[0] -> sourceToken);
        printMul(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printMul(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Boolean NA for operator");
        // printf("ERROR: %2d %25s %3s %20s %2d %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "|||", t->token->symbol, prev->expression.primitive, "***", "***", t -> depth, "invalid type for operator");
    }
    else {
        head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[0] -> typeExpression;
    }
    stack top = head;
    head = pop(head);
    free(top);
}

void handleAndLogicExpr(parseNode* t, typeExpressionTable T) {
    handleAddLogicExpr(t -> children[0], T);
    t -> token -> lineNumber = t -> children[0] -> token -> lineNumber;
    if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
        t -> typeExpression = t -> children[0] -> typeExpression;
        return;
    }
    stack head = malloc(sizeof(struct parserStack));
    head -> parseTreeNode = t;
    head -> next = NULL;
    parseNode* temp = t -> children[1]; // andLogicOptional
    while(temp -> num_child == 3) {
        stack node = malloc(sizeof(struct parserStack));
        node -> parseTreeNode = temp;
        node -> next = NULL;
        head = push(node, head);
        temp = temp -> children[2];
    }
    typeExpressionRow* prev = NULL;
    while(head -> parseTreeNode != t) {
        handleAddLogicExpr(head -> parseTreeNode -> children[1], T);
        if(prev == NULL) {
            head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
            prev = head -> parseTreeNode -> typeExpression;
        }
        else {
            if(prev -> typeVariable == ERROR) {
                //Error propagation
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable == ERROR) {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
                prev = head -> parseTreeNode -> children[1] -> typeExpression;
            }
            else if(prev -> typeVariable != PRIMITIVE || prev -> expression.primitive != BOOLEAN_TYPE) {
                //error
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> sourceToken);
                printAdd(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printAdd(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Boolean required for operator");
                // printf("ERROR: %2d %25s %3s %20s %2d %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", "|||", t->token->symbol, prev->expression.primitive, "***", "***", t -> depth, "invalid type for operator");
                //set prev as error
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE; 
                head -> parseTreeNode -> typeExpression = prev;
            }
           else if (head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable != PRIMITIVE || (head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive != BOOLEAN_TYPE) {
               //error 
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> sourceToken);
                printAdd(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printAdd(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Boolean required for operator");
                // printf("ERROR: %2d %25s *** %20s %2d %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", head -> parseTreeNode -> children[1] ->token -> symbol, head -> parseTreeNode -> children[1] -> typeExpression -> expression.primitive, "***", "***", t -> depth, "invalid type for operator");
                //set prev as error
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE; 
                head -> parseTreeNode -> typeExpression = prev;
           }
            else  {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
                prev -> expression = head -> parseTreeNode -> typeExpression -> expression;
                prev -> rectArrayType = head -> parseTreeNode -> typeExpression -> rectArrayType;
                prev -> typeVariable = head -> parseTreeNode -> typeExpression -> typeVariable;
            }
        }
        stack top = head;
        head = pop(head);
        free(top);
    }
    if (prev -> typeVariable == ERROR) {
        //Error propagation
        head -> parseTreeNode -> typeExpression = prev;
    }
    else if(prev -> typeVariable != PRIMITIVE || prev -> expression.primitive != BOOLEAN_TYPE) {
        //error
        t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] -> sourceToken);
        printAdd(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printAdd(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Boolean required for operator");
        // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, prev -> expression.primitive, "***", "***", t -> depth, "invalid type for operator");
    }
    else if(head -> parseTreeNode -> children[0] -> typeExpression -> typeVariable != PRIMITIVE || (head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive != BOOLEAN_TYPE) {
        //error
        t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] -> sourceToken);
        printAdd(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printAdd(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Boolean required for operator");
        // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", head -> parseTreeNode -> children[0] -> token -> symbol, (head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive, "***", "***", t -> depth, "invalid type for operator");
    }
    else {
        head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[0] -> typeExpression;
    }
    stack top = head;
    head = pop(head);
    free(top);
}

void handleOrLogicExpr(parseNode* t, typeExpressionTable T) {
    //t = orLogicExpr
    handleAndLogicExpr(t -> children[0], T);
    t -> token -> lineNumber = t -> children[0] -> token -> lineNumber;
    if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
        t -> typeExpression = t -> children[0] -> typeExpression;
        return;
    }
    stack head = malloc(sizeof(struct parserStack));
    head -> parseTreeNode = t;
    head -> next = NULL;
    parseNode* temp = t -> children[1]; // orLogicOptional
    while(temp -> num_child == 3) {
        stack node = malloc(sizeof(struct parserStack));
        node -> parseTreeNode = temp;
        node -> next = NULL;
        head = push(node, head);
        temp = temp -> children[2];
    }
    typeExpressionRow* prev = NULL;
    while(head -> parseTreeNode != t) {
        handleAndLogicExpr(head -> parseTreeNode -> children[1], T);
        if(prev == NULL) {
            head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
            prev = (typeExpressionRow*)malloc(sizeof(typeExpressionRow));
            prev -> expression = head -> parseTreeNode -> typeExpression -> expression;
            prev -> rectArrayType = head -> parseTreeNode -> typeExpression -> rectArrayType;
            prev -> typeVariable = head -> parseTreeNode -> typeExpression -> typeVariable;
        }
        else {
            if(prev -> typeVariable == ERROR) {
                //Error propagation
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(prev -> typeVariable != PRIMITIVE || prev -> expression.primitive != BOOLEAN_TYPE) {
                //error
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> sourceToken);
                printAnd(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printAnd(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Boolean required for operator");
                // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", t->token->symbol, prev -> expression.primitive, "***", "***", t -> depth, "invalid type for operator");
                //set prev as error
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE;
                head -> parseTreeNode -> typeExpression = prev; 
            }
           else if (head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable != PRIMITIVE || (head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive != BOOLEAN_TYPE) {
                //error
                printf("ERROR: %2d %25s  ", head -> parseTreeNode -> children[0] -> token -> lineNumber, "Assignment statement");
                printf("%5s ", head -> parseTreeNode -> children[0] -> sourceToken);
                printAnd(head -> parseTreeNode -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[1] -> typeExpression);
                printAnd(head -> parseTreeNode -> children[2] -> children[1]);
                printTypeExpression(head -> parseTreeNode -> children[2] -> children[1] -> typeExpression);
                printf("%3d %30s\n", head -> parseTreeNode -> depth, "Boolean required for operator");
                // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", head -> parseTreeNode -> children[1]->token->symbol, (head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive, "***", "***", t -> depth, "invalid type for operator");
                //set prev as error
                prev = malloc(sizeof(typeExpressionRow));
                prev -> typeVariable = ERROR;
                prev -> rectArrayType = NOT_APPLICABLE;
                prev -> expression.primitive = INTEGER_TYPE;
                head -> parseTreeNode -> typeExpression = prev;
           }
            else  {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
                prev -> expression = head -> parseTreeNode -> typeExpression -> expression;
                prev -> rectArrayType = head -> parseTreeNode -> typeExpression -> rectArrayType;
                prev -> typeVariable = head -> parseTreeNode -> typeExpression -> typeVariable;
            }
        }
        stack top = head;
        head = pop(head);
        free(top);
    }
    if(prev -> typeVariable != PRIMITIVE || prev -> expression.primitive != BOOLEAN_TYPE) {
        t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] -> sourceToken);
        printAnd(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printAnd(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Boolean required for operator");
        // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", head -> parseTreeNode -> children[1]->token->symbol, (head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive, "***", "***", t -> depth, "invalid type for operator");
    }
    else if(head -> parseTreeNode -> children[0] -> typeExpression -> typeVariable != PRIMITIVE || (head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive != BOOLEAN_TYPE) {
        //error
        t -> typeExpression = malloc(sizeof(typeExpressionRow));
        t -> typeExpression -> typeVariable = ERROR;
        t -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        t -> typeExpression -> expression.primitive = INTEGER_TYPE;
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s ", t -> children[1] -> children[0] -> sourceToken);
        printAnd(t -> children[0]);
        printTypeExpression(t -> children[0] -> typeExpression);
        printAnd(t -> children[1] -> children[1]);
        printTypeExpression(t -> children[1] -> children[1] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "Boolean required for operator");
        // printf("ERROR: %2d %25s *** %20s %10s %20s %10s %3d %30s\n", t -> token -> lineNumber, "Assignment statement", head -> parseTreeNode -> children[1]->token->symbol, (head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive, "***", "***", t -> depth, "invalid type for operator");
    }
    else {
        head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[0] -> typeExpression;
    }
    stack top = head;
    head = pop(head);
    free(top);
}

void handleAssignmentStmt(parseNode* t, typeExpressionTable T) {
    // t -> children[2] expression
    // t - children[0] element
    handleElement(t -> children[0], T);
    t -> token -> lineNumber = t -> children[0] -> token -> lineNumber;
    handleOrLogicExpr(t -> children[2] -> children[0], T);
    t -> children[2] -> typeExpression = t -> children[2] -> children[0] -> typeExpression;
    if(t -> children[2] -> typeExpression -> typeVariable == ERROR || t -> children[0] -> typeExpression -> typeVariable == ERROR) {   
        t -> typeExpression = t -> children[2] -> typeExpression;
    }
    else if(compTypeExp(t -> children[0] -> typeExpression, t -> children[2] -> typeExpression) == 0) {
        //error
        printf("ERROR: %2d %25s  ", t -> token -> lineNumber, "Assignment statement");
        printf("%5s %10s ", t -> children[1] -> sourceToken, t -> children[0] -> children[0] -> sourceToken);
        printTypeExpression(t -> children[0] -> typeExpression);
        printTerm(t -> children[2] -> children[0] -> children[0] -> children[0] -> children[0] -> children[0]);
        printTypeExpression(t -> children[2] -> typeExpression);
        printf("%3d %30s\n", t -> depth, "type mismatch");
    }
    else {
        t -> typeExpression = t -> children[0] -> typeExpression;
    }
}

void traverseParseTree(parseTree *t, typeExpressionTable T) {
    /*
        start -> stmts
    */
    // declarativeStmts
    parseNode* temp = t -> children[4] -> children[0]; // this points to "declarativeStmts"
    while (temp -> num_child == 2) {
        //Cannot be epsilon
        handleDeclarativeStmt(temp -> children[0], T);        
        temp = temp -> children[1];
    }

    // assignmentStmts
    temp = t -> children[4] -> children[1]; // this points to "assignmentStmts"
     while (temp -> num_child == 2) {
        //Cannot be epsilon
        handleAssignmentStmt(temp -> children[0], T);   //assignmentStmt
        temp = temp -> children[1];
    }
   //
}