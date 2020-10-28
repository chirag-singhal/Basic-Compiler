#include "typeExpression.h"

#define MAX_SIZE 50

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
    if (T == NULL) {
        T = (typeExpressionTable)malloc(sizeof(typeExpressionTableNode));
        T -> symbol = malloc(sizeof(char) * MAX_SIZE);
        strcpy(T -> symbol, symbol);
        T -> row = row;
    }
    T -> next = (typeExpressionTable)malloc(sizeof(typeExpressionTableNode));
    T -> next -> prev = T;
    T = T -> next;
    // T -> symbol = malloc(sizeof(char) * MAX_SIZE);
    // strcpy(T -> symbol, symbol);
    T -> symbol = symbol;
    T -> row = row;
}

void handleDeclarativeStmt(parseNode* t, typeExpressionTable T) {
    /*
        if varOrListDeclare, check first child
        if VAR_ID, add only 1 row
        else add multiple 
    */
    // t -> children[1] Specific Declarative Phrase
    if(t -> children[1] -> children[0] -> token -> tokenID == primitiveDeclarativePhrase) {
        //Primitive
        t -> children[1] -> typeExpression -> typeVariable = PRIMITIVE;
        t -> children[1] -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        if(t -> children[1] -> children[0] -> children[0] -> children[0] -> token -> tokenID == INTEGER) {
            //INTEGER
            (t -> children[1] -> typeExpression -> expression).primitive = INTEGER;
        }
        else if (t -> children[1] -> children[0] -> children[0] -> children[0] -> token -> tokenID == BOOLEAN) {
            //BOOLEAN
            (t -> children[1] -> typeExpression -> expression).primitive = BOOLEAN;
        }
        else {
            //REAL
            (t -> children[1] -> typeExpression -> expression).primitive = REAL;
        }
        
        // push variable to type expression table
        if (t -> children[0] -> children[1] -> num_child == 1) {
            // t -> children[0] -> children[1] -> children[0] is the parse node pointing to the VAR_ID
            push_typeExpTable(t -> children[0] -> children[1] -> children[0] -> token -> symbol, t -> children[1] -> typeExpression, T);
        }
        else {
            // t -> children[0] -> children[1] -> children[3] is the parse node pointing to listVarId
            parseNode* temp = t -> children[0] -> children[1] -> children[3];
            while (temp -> num_child == 2) {
                push_typeExpTable(temp -> children[0] -> token -> symbol, t -> children[1] -> typeExpression, T);
                temp = temp -> children[0];
            }
        }
    }
    else if(t -> children[1] -> children[0] -> token -> tokenID == rectArrayDeclarativePhrase) {
        //RectArray INTEGER
        t -> children[1] -> typeExpression -> typeVariable = RECTARRAY;

        int numOfDim = 0;
        enum _typeRectArrayEnum rectArrayType = STATIC;
        struct rectArray rectArr;
        rectArr.ranges = (rectArrayIndex**) malloc(sizeof(rectArrayIndex) * 32 * 2);

        parseNode* temp = t -> children[1] -> children[0] -> children[1]; // listDim
        
        while(temp -> num_child != 1) {
            // first range index
            if(temp -> children[1] -> children[0] -> token -> tokenID == VAR_ID) {
                // check if VAR_ID is INTEGER from type expression table
                typeExpressionRow* row = search_typeExpTable(temp -> children[1] -> children[0] -> token -> symbol, T);
                if (row -> typeVariable != PRIMITIVE || (row -> expression).primitive != INTEGER) {
                    // error
                    //lineNumber, statement type, operator, firstOperandLexeme, type,
                    //secondOperandLexeme, type, position in parse tree, message 
                    printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d RA type mismatch\n", t -> token -> lineNumber, t -> depth);
                }
                rectArr.ranges[numOfDim][0].isVarId = 1;
                rectArr.ranges[numOfDim][0].value.varIdIndex = temp -> children[1] -> children[0] -> token -> symbol;
            }
            else {
                rectArr.ranges[numOfDim][0].isVarId = 0;
                rectArr.ranges[numOfDim][0].value.intIndex = atoi(temp -> children[1] -> children[0] -> token -> symbol);
            }
            // second range index
            if(temp -> children[3] -> children[0] -> token -> tokenID == VAR_ID) {
                typeExpressionRow* row = search_typeExpTable(temp -> children[3] -> children[0] -> token -> symbol, T);
                if (row -> typeVariable != PRIMITIVE || (row -> expression).primitive != INTEGER) {
                    // error
                    printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d RA type mismatch\n", t->token->lineNumber, t->depth);
                }
                rectArr.ranges[numOfDim][1].isVarId = 1;
                rectArr.ranges[numOfDim][1].value.varIdIndex = temp -> children[1] -> children[0] -> token -> symbol;
            }
            else {
                rectArr.ranges[numOfDim][1].isVarId = 0;
                rectArr.ranges[numOfDim][1].value.intIndex = atoi(temp -> children[1] -> children[0] -> token -> symbol);
            }

            if(temp -> children[1] -> children[0] -> token -> tokenID == VAR_ID || temp -> children[3] -> children[0] -> token -> tokenID == VAR_ID)
                rectArrayType = DYNAMIC;
            numOfDim++;
            temp = temp -> children[5];
        }
        t -> children[1] -> typeExpression -> rectArrayType = rectArrayType;
        rectArr.dimensions = numOfDim;
        rectArr.basicElementType = INTEGER;
        (t -> children[1] -> typeExpression -> expression).rectArray = rectArr;
        
        // push variable to type expression table
        if (t -> children[0] -> children[1] -> num_child == 1) {
            // t -> children[0] -> children[1] -> children[0] is the parse node pointing to the VAR_ID
            push_typeExpTable(t -> children[0] -> children[1] -> children[0] -> token -> symbol, t -> children[1] -> typeExpression, T);
        }
        else {
            // t -> children[0] -> children[1] -> children[3] is the parse node pointing to listVarId
            parseNode* temp = t -> children[0] -> children[1] -> children[3];
            while (temp -> num_child == 2) {
                push_typeExpTable(temp -> children[0] -> token -> symbol, t -> children[1] -> typeExpression, T);
                temp = temp -> children[0];
            }
        }
    }
    else{
        //jaggedArray INTEGER
        t -> children[1] -> typeExpression -> typeVariable = JAGGEDARRAY;
        t -> children[1] -> typeExpression -> rectArrayType = NOT_APPLICABLE;
        int numDim;
        // temp is the pointer to listEmptyDim
        parseNode* temp = t -> children[1] -> children[0] -> children[3];
        if(temp -> children[2] -> num_child == 1)
            numDim = 2;
        else // if(temp -> children[2] -> num_child == 3)
            numDim = 3;
        
        struct jaggedArray jaggedArray;
        jaggedArray.dimensions = numDim;
        if (t -> children[1] -> children[0] -> children[2] -> children[1] -> children[0] -> token -> tokenID == VAR_ID || t -> children[1] -> children[0] -> children[2] -> children[3] -> children[0] -> token -> tokenID == VAR_ID) {
            // error
            printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d JA invalid range\n", t->token->lineNumber, t->depth);
        }
        jaggedArray.R1_range[0] = atoi(t -> children[1] -> children[0] -> children[2] -> children[1] -> children[0] -> token -> symbol);
        jaggedArray.R1_range[1] = atoi(t -> children[1] -> children[0] -> children[2] -> children[3] -> children[0] -> token -> symbol);
        
        if (numDim == 2)
            jaggedArray.R2_ranges.range_2d = (int*)malloc((jaggedArray.R1_range[1]-jaggedArray.R1_range[0]+1) * sizeof(int));
        else
            jaggedArray.R2_ranges.range_3d = (int**)malloc((jaggedArray.R1_range[1]-jaggedArray.R1_range[0]+1) * sizeof(int*));
        
        jaggedArray.basicElementType = INTEGER;
        // we need to count for third dimension
        // now, temp is the pointer to jaggedArrayInitialization
        temp = t -> children[1] -> children[1];

        while (temp -> num_child == 2) {
            temp -> children[0] -> children[2]; // R1 : NUM
            temp -> children[0] -> children[6]; // SIZE : NUM
            temp -> children[0] -> children[10]; // pointer to listIntSemiColon
            // do something
            if (numDim == 2)
                jaggedArray.R2_ranges.range_2d[atoi(temp -> children[0] -> children[2] -> token -> symbol)-jaggedArray.R1_range[0]] = atoi(temp -> children[0] -> children[6] -> token -> symbol);
            else {
                jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> token -> symbol)-jaggedArray.R1_range[0]] = (int*)malloc((atoi(temp -> children[0] -> children[6] -> token -> symbol)+1) * sizeof(int));
            }
            
            parseNode* temp2 = temp -> children[0] -> children[10];
            // temp2 points to listIntSemiColon
            if (numDim == 3)
                jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> token -> symbol)-jaggedArray.R1_range[0]][0] = atoi(temp -> children[0] -> children[6]);
            int count = 0;
            while (temp2 -> num_child == 3 || temp2 -> num_child == 2) {
                if (numDim == 2 && count >= jaggedArray.R2_ranges.range_2d[atoi(temp -> children[0] -> children[2] -> token -> symbol)-jaggedArray.R1_range[0]]) {
                    // error
                    printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d 2D JA excess elements\n", t->token->lineNumber, t->depth);
                }
                if (numDim == 3 && count >= jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> token -> symbol)-jaggedArray.R1_range[0]][0]) {
                    // error
                    printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d 3D JA excess elements\n", t->token->lineNumber, t->depth);
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
                }
                else if (numDim == 3) {
                    jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> token -> symbol)-jaggedArray.R1_range[0]][count+1] = count2;
                }
                count++;
                temp2 = temp2 -> children[temp2 -> num_child-1];
            }
            if (numDim == 2 && count < jaggedArray.R2_ranges.range_2d[atoi(temp -> children[0] -> children[2] -> token -> symbol)-jaggedArray.R1_range[0]]) {
                // error
            }
            if (numDim == 3 && count < jaggedArray.R2_ranges.range_3d[atoi(temp -> children[0] -> children[2] -> token -> symbol)-jaggedArray.R1_range[0]][0]) {
                // error
            }
            //
            temp = temp -> children[1];
        }
        (t -> children[1] -> typeExpression -> expression).jaggedArray = jaggedArray;

        // push variable to type expression table
        if (t -> children[0] -> children[1] -> num_child == 1) {
            // t -> children[0] -> children[1] -> children[0] is the parse node pointing to the VAR_ID
            push_typeExpTable(t -> children[0] -> children[1] -> children[0] -> token -> symbol, t -> children[1] -> typeExpression, T);
        }
        else {
            // t -> children[0] -> children[1] -> children[3] is the parse node pointing to listVarId
            parseNode* temp = t -> children[0] -> children[1] -> children[3];
            while (temp -> num_child == 2) {
                push_typeExpTable(temp -> children[0] -> token -> symbol, t -> children[1] -> typeExpression, T);
                temp = temp -> children[0];
            }
        }
    }
}

int compTypeExp(typeExpressionRow* r1, typeExpressionRow* r2){
    if(r1 -> typeVariable != r2 -> typeVariable)
        return 0;
    else if(r1 -> rectArrayType != r2 -> rectArrayType)
        return 0;
    else
    {
        //primitive
        if(r1 -> typeVariable == PRIMITIVE)
        {
            if((r1 -> expression).primitive != (r2 -> expression).primitive)
                return 0;
        }
        //rectangular
        else if(r1 -> typeVariable == RECTARRAY)
        {
            int dim = (r1->expression).rectArray.dimensions;
            if((r1->expression).rectArray.dimensions != (r2->expression).rectArray.dimensions)
                return 0;
            else if((r1->expression).rectArray.basicElementType != (r1->expression).rectArray.basicElementType)
                return 0;
            else if((r1->expression).rectArray.ranges[dim][0].isVarId != (r2->expression).rectArray.ranges[dim][0].isVarId)
                return 0;
            else if((r1->expression).rectArray.ranges[dim][1].isVarId != (r2->expression).rectArray.ranges[dim][1].isVarId)
                return 0;
            int v1 = (r1->expression).rectArray.ranges[dim][0].isVarId;
            int v2 = (r1->expression).rectArray.ranges[dim][1].isVarId;
            for(int i=0; i<dim; i++){
                //low...high
                //for low
                if(v1 == 0)
                {
                    if((r1->expression).rectArray.ranges[i][0].value.intIndex != (r2->expression).rectArray.ranges[i][0].value.intIndex)
                        return 0;
                }
                else if(v1 == 1)
                {
                    if(!strcmp((r1->expression).rectArray.ranges[i][0].value.intIndex, (r2->expression).rectArray.ranges[i][0].value.intIndex))
                        return 0;
                }
                //for high
                if(v1 == 0)
                {
                    if((r1->expression).rectArray.ranges[i][1].value.intIndex != (r2->expression).rectArray.ranges[i][1].value.intIndex)
                        return 0;
                }
                else if(v1 == 1)
                {
                    if(!strcmp((r1->expression).rectArray.ranges[i][1].value.intIndex, (r2->expression).rectArray.ranges[i][1].value.intIndex))
                        return 0;
                }
            }
        }
        //jagged
        else if(r1 -> typeVariable == JAGGEDARRAY)
        {
            int dim = (r1->expression).jaggedArray.dimensions;
            if((r1->expression).jaggedArray.dimensions != (r2->expression).jaggedArray.dimensions)
                return 0;
            else if((r1->expression).jaggedArray.basicElementType != (r1->expression).jaggedArray.dimensions)  //not required
                return 0;
            else if((r1->expression).jaggedArray.R1_range[0]!=(r2->expression).jaggedArray.R1_range[0] || (r1->expression).jaggedArray.R1_range[1]!=(r2->expression).jaggedArray.R1_range[1])
                return 0;
            if(dim == 2)
            {
                int num = (r1->expression).jaggedArray.R1_range[1] - (r1->expression).jaggedArray.R1_range[0];
                for(int i=0; i<num; i++)
                {
                    if((r1->expression).jaggedArray.R2_ranges.range_2d[i] != (r2->expression).jaggedArray.R2_ranges.range_2d[i])
                        return 0;
                }
            }
            else if(dim == 3)
            {
                int dim2 = (r1->expression).jaggedArray.R1_range[1] - (r1->expression).jaggedArray.R1_range[0];
                for(int i = 0; i < dim2; i++)
                {
                    int size1 = (r1->expression).jaggedArray.R2_ranges.range_3d[i][0];
                    int size2 = (r2->expression).jaggedArray.R2_ranges.range_3d[i][0];
                    if(size1 != size2)
                        return 0;
                    for(int j=1; j<size1; j++)
                    {
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
    typeExpressionRow* row = search_typeExpTable(t -> children[0] -> token -> symbol, T);
    if (t -> children[1] -> num_child == 1) {
        // single variable
        t -> typeExpression = row;
    }
    else {
        // Array of type INTEGER
        // temp points to listInt
        parseNode* temp = t -> children[1] -> children[1];
        int dim = 0;

        // if any error, set this new_row to be ERROR and return
        typeExpressionRow* new_row = (typeExpressionRow*)malloc(sizeof(typeExpressionRow));
        
        if (row -> typeVariable == RECTARRAY) {
            while (temp -> num_child == 2) {
                // check idx with ArrayBounds[dim]
                if (dim >= (row -> expression).rectArray.dimensions) {
                    // error
                    printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d RA index out of bounds\n", t->token->lineNumber, t->depth);
                }
                
                if (temp -> children[0] -> children[0] -> token -> tokenID == NUM) {
                    int idx = atoi(temp -> children[0] -> children[0] -> token -> symbol);
                    if ((row -> expression).rectArray.ranges[dim][0].isVarId && getToken((row -> expression).rectArray.ranges[dim][0].value.varIdIndex) != VAR_ID) {
                        // error - set type expression to ERROR
                        printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d undefined variable type\n", t->token->lineNumber, t->depth);
                    }
                    else if (!(row -> expression).rectArray.ranges[dim][0].isVarId && (row -> expression).rectArray.ranges[dim][0].value.intIndex > idx) {
                        // error
                        printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d index out of bounds\n", t->token->lineNumber, t->depth);
                    }
                    else if ((row -> expression).rectArray.ranges[dim][1].isVarId && getToken((row -> expression).rectArray.ranges[dim][1].value.varIdIndex) != VAR_ID) {
                        // error
                        printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d undefined variable type\n", t->token->lineNumber, t->depth);
                    }
                    else if (!(row -> expression).rectArray.ranges[dim][1].isVarId && (row -> expression).rectArray.ranges[dim][1].value.intIndex < idx) {
                        // error
                        printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d index out of bounds\n", t->token->lineNumber, t->depth);
                    }
                }
                else 
                dim++;           
                //
                temp = temp -> children[1];
            }
            if (dim < (row -> expression).rectArray.dimensions) {
                // error
                printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d RA too few elements\n", t->token->lineNumber, t->depth);
            }

            new_row -> typeVariable = PRIMITIVE;
            new_row -> rectArrayType = NOT_APPLICABLE;
            (new_row -> expression).primitive = INTEGER;
            t -> typeExpression = new_row;
        }

        //temp points to listInt
        else if(row -> typeVariable == JAGGEDARRAY) {
            // handle only R1 range
            if (temp -> children[0] -> children[0] -> token -> tokenID == NUM) {
                int idx = atoi(temp -> children[0] -> children[0] -> token -> symbol);
                if (idx < (row -> expression).jaggedArray.R1_range[0] || idx > (row -> expression).jaggedArray.R1_range[1]) {
                    // error
                    printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d RA index out of bounds\n", t->token->lineNumber, t->depth);
                }
                temp = temp -> children[1];

                if ((row -> expression).jaggedArray.dimensions == 2) {
                    // 2 dim jagged array
                    if (temp -> children[1] -> num_child == 2) {
                        // error
                        printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d 2D JA incorrect dim number\n", t->token->lineNumber, t->depth);
                    }
                    if (temp -> children[1] -> children[0] -> token -> tokenID == NUM) {
                        int idx2 = atoi(temp -> children[1] -> children[0] -> children[0] -> token -> symbol);
                        if(idx2 > (row -> expression).jaggedArray.R2_ranges.range_2d[idx - (row -> expression).jaggedArray.R1_range[0]] - 1) {
                            //error
                            printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d 3D JA excess elements\n", t->token->lineNumber, t->depth);
                        }
                    }
                }
                else {
                    // 3 dim jagged array
                    if (temp -> children[1] -> num_child == 1) {
                        // error
                        printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d 3D JA too few dimensions\n", t->token->lineNumber, t->depth);
                    }
                    //R2 range
                    if (temp -> children[1] -> children[0] -> children[0] -> token -> tokenID == NUM) {
                        int idx2 = atoi(temp -> children[1] -> children[0] -> children[0] -> token -> symbol);
                        if(idx2 > (row -> expression).jaggedArray.R2_ranges.range_3d[idx - (row -> expression).jaggedArray.R1_range[0]][0] - 1) {
                            //error
                            printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d 3D JA index out of bounds\n", t->token->lineNumber, t->depth);
                        }
                    
                        //R3 range
                        if (temp -> children[1] -> children[1] -> children[0] -> children[0] -> token -> tokenID) {
                            int idx3 = atoi(temp -> children[1] -> children[1] -> children[0] -> children[0] -> token -> symbol);
                            if(idx3 > (row -> expression).jaggedArray.R2_ranges.range_3d[idx - (row -> expression).jaggedArray.R1_range[0]][idx2] - 1) {
                                //error
                                printf("ERROR: %2d Declaration Statement *** *** *** *** *** %3d 3D JA index out of bounds\n", t->token->lineNumber, t->depth);
                            }
                        }
                    }
                }
            }

            new_row -> typeVariable = PRIMITIVE;
            new_row -> rectArrayType = NOT_APPLICABLE;
            (new_row -> expression).primitive = INTEGER;
            t -> typeExpression = new_row;
        }
        else {
            //error
        }
    }
}

void handleMulLogicExpr(parseNode* t, typeExpressionTable T) {
    handleTerm(t -> children[0], T);
    if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
        t -> typeExpression = t -> children[1] -> typeExpression;
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
            prevMulOp = head -> parseTreeNode -> children[0] -> token -> tokenID == MUL_OP;
        }
        else {
            if(prev -> typeVariable == ERROR) {
                //Error propagation
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(prev -> expression.primitive == BOOLEAN) {
                //error
            }
            else if(compTypeExp(prev, head -> parseTreeNode -> children[1] -> typeExpression) == 0) {
                //error
            }
            else if ((head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive == BOOLEAN) {
               //error 
            }
            else  {
                if(prevMulOp) {
                    head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
                }
                else {
                    typeExpressionRow* typeExpression = malloc(sizeof(typeExpressionRow));
                    typeExpression -> expression = head -> parseTreeNode -> children[1] -> typeExpression -> expression;
                    typeExpression -> rectArrayType = head -> parseTreeNode -> children[1] -> typeExpression -> rectArrayType;
                    typeExpression -> typeVariable = head -> parseTreeNode -> children[1] -> typeExpression -> typeVariable;
                    if(typeExpression -> typeVariable == PRIMITIVE)
                        typeExpression -> expression.primitive = REAL;
                    else if(typeExpression -> typeVariable == RECTARRAY)
                        typeExpression -> expression.rectArray.basicElementType = REAL;
                    else 
                        typeExpression -> expression.jaggedArray.basicElementType = REAL;
                }
            }
        }
        prev = head -> parseTreeNode -> typeExpression;
        stack top = head;
        head = pop(head);
        free(top);
    }
    if(prev -> expression.primitive != BOOLEAN) {
        //error
    }
    else if(compTypeExp(prev, head -> parseTreeNode -> children[0] -> typeExpression) == 0) {
        //error
    }
    else if((head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive == BOOLEAN) {
        //error
    }
    else {
        head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[0] -> typeExpression;
    }
    stack top = head;
    head = pop(head);
    free(top);
}

void handleAddLogicExpr(parseNode* t, typeExpressionTable T) {
    handleMulLogicExpr(t -> children[0], T);
    if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
        t -> typeExpression = t -> children[1] -> typeExpression;
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
        handleAndLogicExpr(head -> parseTreeNode -> children[1], T);
        if(prev == NULL) 
            head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
        else {
            if(prev -> typeVariable == ERROR) {
                //Error propagation
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(prev -> expression.primitive == BOOLEAN) {
                //error
            }
            else if(compTypeExp(prev, head -> parseTreeNode -> children[1] -> typeExpression) == 0) {
                //error
            }
            else if ((head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive == BOOLEAN) {
               //error 
            }
            else  {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
            }
        }
        prev = head -> parseTreeNode -> typeExpression;
        stack top = head;
        head = pop(head);
        free(top);
    }
    if(prev -> expression.primitive != BOOLEAN) {
        //error
    }
    else if(compTypeExp(prev, head -> parseTreeNode -> children[0] -> typeExpression) == 0) {
        //error
    }
    else if((head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive == BOOLEAN) {
        //error
    }
    else {
        head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[0] -> typeExpression;
    }
    stack top = head;
    head = pop(head);
    free(top);
}

void handleAndLogicExpr(parseNode* t, typeExpressionTable T) {
    // handleAddLogicExpr(t -> children[0], T);
    // if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
    //     t -> typeExpression = t -> children[1] -> typeExpression;
    //     return;
    // }
    // if((t -> children[0] -> typeExpression -> expression).primitive != BOOLEAN) {
    //     //error
            
    //     //return
    // }
    // parseNode* temp = t -> children[1]; // andLogicOptional
    // while(temp -> num_child == 3) {
    //     handleAddLogicExpr(temp -> children[2], T);
    //     if((temp -> children[2] -> typeExpression -> expression).primitive != BOOLEAN) {
    //         //error
            
    //         //return
    //     }
    //     temp = temp -> children[3];
    // }
    // set typeExpression as boolean

    handleAddLogicExpr(t -> children[0], T);
    if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
        t -> typeExpression = t -> children[1] -> typeExpression;
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
        handleAndLogicExpr(head -> parseTreeNode -> children[1], T);
        if(prev == NULL) 
            head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
        else {
            if(prev -> typeVariable == ERROR) {
                //Error propagation
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(prev -> expression.primitive != BOOLEAN) {
                //error
            }
           else if ((head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive != BOOLEAN) {
               //error 
           }
            else  {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
            }
        }
        prev = head -> parseTreeNode -> typeExpression;
        stack top = head;
        head = pop(head);
        free(top);
    }
    if(prev -> expression.primitive != BOOLEAN) {
        //error
    }
    else if((head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive != BOOLEAN) {
        //error
    }
    else {
        head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[0] -> typeExpression;
    }
    stack top = head;
    head = pop(head);
    free(top);
}

void handleOrLogicExpr(parseNode* t, typeExpressionTable T) {
    handleAndLogicExpr(t -> children[0], T);
    if(t -> children[1] -> num_child == 1 || t -> children[0] -> typeExpression -> typeVariable == ERROR) {
        t -> typeExpression = t -> children[1] -> typeExpression;
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
        if(prev == NULL) 
            head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
        else {
            if(prev -> typeVariable == ERROR) {
                //Error propagation
                head -> parseTreeNode -> typeExpression = prev;
            }
            else if(prev -> expression.primitive != BOOLEAN) {
                //error
            }
           else if ((head -> parseTreeNode -> children[1] -> typeExpression -> expression).primitive != BOOLEAN) {
               //error 
           }
            else  {
                head -> parseTreeNode -> typeExpression = head -> parseTreeNode -> children[1] -> typeExpression;
            }
        }
        prev = head -> parseTreeNode -> typeExpression;
        stack top = head;
        head = pop(head);
        free(top);
    }
    if(prev -> expression.primitive != BOOLEAN) {
        //error
    }
    else if((head -> parseTreeNode -> children[0] -> typeExpression -> expression).primitive != BOOLEAN) {
        //error
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
    handleOrLogicExpr(t -> children[2] -> children[0], T);
    t -> children[2] -> typeExpression = t -> children[2] -> children[0] -> typeExpression;
    if(compTypeExp(t -> children[0] -> typeExpression, t -> children[2] -> typeExpression) == 0) {
        //error
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
    parseNode* temp = t -> children[0] -> children[0]; // this points to "declarativeStmts"
    while (temp -> num_child == 2) {
        //Cannot be epsilon
        handleDeclarativeStmt(temp -> children[0], T);
        temp = temp -> children[1];
    }
   //

   // assignmentStmts
    temp = t -> children[0] -> children[1]; // this points to "assignmentStmts"
     while (temp -> num_child == 2) {
        //Cannot be epsilon
        handleAssignmentStmt(temp -> children[0], T);
        temp = temp -> children[1];
    }
   //
}