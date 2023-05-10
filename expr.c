//
// Created by Lukáš on 30.09.2022.
//

#include "expr.h"
#include "global_def.h"
#include "lex_analyz.h"
#include "code_gen.h"
#include "SmallMC.h"
#include <string.h>
#include <stdlib.h>

char precedenceTable[PRECEDENCE_TABLE_SIZE][PRECEDENCE_TABLE_SIZE] = {
//         +   |  - |  *  |  (  |  )  |  i  |  $  |  /  |  !== |  <  |  >  |  <=  |  >=   | =  | str
        { '>' , '>' , '<' ,  '<' , '>' , '<' , '>' , '<' ,  '>' , '>' , '>' , '>'  , '>'  , '>' , '<' },   // +
        { '>' , '>' , '<' ,  '<' , '>' , '<' , '>' , '<' ,  '>' , '>' , '>' , '>'  , '>'  , '>' , 'E'},   // -
        { '>' , '>' , '>' ,  '<' , '>' , '<' , '>' , '>' ,  '>' , '>' , '>' , '>'  , '>'  , '>' , 'E' },   // *
        { '<' , '<' , '<' ,  '<' , '=' , '<' , 'E' , '<' ,  '<' , '<' , '<' , '<'  , '<'  , '<' , '<'},   // (
        { '>' , '>' , '>' ,  'E' , '>' , 'E' , '>' , '>' ,  '>' , '>' , '>' , '>'  , '>'  , '>' , 'E'},   // )
        { '>' , '>' , '>' ,  'E' , '>' , 'E' , '>' , '>' ,  '>' , '>' , '>' , '>'  , '>'  , '>' , 'E' },   // i
        { '<' , '<' , '<' ,  '<' , 'E' , '<' , 'E' , '<' ,  '<' , '<' , '<' , '<'  , '<'  , '<' , '<'},   // $
        { '>' , '>' , '>' ,  '<' , '>' , '<' , '>' , '>' ,  '>' , '>' , '>' , '>'  , '>'  , '>' , 'E'},   // /
        { '<' , '<' , '<' ,  '<' , '>' , '<' , '>' , '<' ,  'E' , 'E' , 'E' , 'E'  , 'E'  , 'E' , '<'},   //  !==
        { '<' , '<' , '<' ,  '<' , '>' , '<' , '>' , '<' ,  'E' , 'E' , 'E' , 'E'  , 'E'  , 'E' , '<'}, // <
        { '<' , '<' , '<' ,  '<' , '>' , '<' , '>' , '<' ,  'E' , 'E' , 'E' , 'E'  , 'E'  , 'E' , '<'}, // >
        { '<' , '<' , '<' ,  '<' , '>' , '<' , '>' , '<' ,  'E' , 'E' , 'E' , 'E'  , 'E'  , 'E' , '<'}, // <=
        { '<' , '<' , '<' ,  '<' , '>' , '<' , '>' , '<' ,  'E' , 'E' , 'E' , 'E'  , 'E'  , 'E' , '<'}, // >=
        { '<' , '<' , '<' ,  '<' , '>' , '<' , '>' , '<' ,  'E' , 'E' , 'E' , 'E'  , 'E'  , 'E' , '<'}, // =
        { '>' , 'E' , 'E' ,  'E' , '>' , 'E' , '>' , 'E' ,  '>' , '>' , '>' , '>'  , '>'  , '>' , 'E'} // str
};

//void InitTStack (TStack *s)
//{
//    s->topPtr = NULL;
//
//    // First token on a stack must be dollar
//    Token* firstToken;
//    firstToken = (Token *) malloc(sizeof(Token));
//    firstToken->type = $DOLLAR;
//    PushTStack(s, firstToken);
//}
//
//void PushTStack (TStack *s, Token *symbol)
//{
//    TSElem *newElemPtr = (TSElem *) malloc(sizeof(TSElem));
//    if(newElemPtr == NULL) {
//        fprintf(stderr, "Malloc error");
//        return;
//    }
//
//    //newElemPtr->data = (Token *) malloc(sizeof(Token));
//    newElemPtr->data = symbol;
//    newElemPtr->nextPtr = s->topPtr;
//    s->topPtr = newElemPtr;
//}
//
//void PopTStack (TStack *s)
//{
//    TSElem *elemPtr;
//    if (s->topPtr != NULL) {
//        elemPtr = s->topPtr;
//        s->topPtr = s->topPtr->nextPtr;
//        free(elemPtr);
//    }
//}
//
//Token* TopTStack (TStack *s)
//{
//    return (s->topPtr->data);
//}
//
//bool IsEmptyTStack (TStack *s)
//{
//    return (s->topPtr == NULL);
//}

void InitTStack(TStack *stack)
{
    if (!stack)
        return;

    stack->activeElement = NULL;
    stack->firstElement = NULL;

    //First token on a stack must be dollar
      Token* firstToken;
      firstToken = (Token *) malloc(sizeof(Token));
      firstToken->type = $DOLLAR;
      firstToken->value.str = "$";
      PushTStack(stack, firstToken);
}

void DisposeTStack(TStack *stack) {
    if (!stack)
        return;

    StackElementPtr temp = stack->firstElement;

    // prochazeni seznamu list a ruseni vsech jeho prvku
    while (stack->firstElement != NULL)
    {
        temp = stack->firstElement;
        stack->firstElement = stack->firstElement->nextElement;
        free(temp);
    }

    // Uvedeni seznamu do stavu, ve kterém se nacházel po inicializaci
    stack->activeElement = NULL;
    stack->firstElement = NULL;
}

void PushTStack(TStack *stack, Token *sign) {
    if (!stack)
        return;

    StackElementPtr newElementPtr = malloc(sizeof(struct StackElement));
    newElementPtr->data = malloc(sizeof(Token));
    newElementPtr->nextElement = NULL;
    memcpy(newElementPtr->data, sign,sizeof(Token)); // nastavení dat u nového prvku seznamu

    if (stack->firstElement != NULL)
        newElementPtr->nextElement = stack->firstElement; // nastavení ukazatele na další prvek

    stack->firstElement = newElementPtr; // nastavení ukazatele na první prvek seznamu na nový prvek
}


// use only for testing
void displayTStack(TStack *stack) {
    StackElementPtr temp = stack->firstElement;
    for(int i = 0; temp != NULL; i++) {
        printf("Type of token of %d element", i);
        printf(" is %c", temp->data->type);
    }
}

// Push shift < after the last terminal
void PushTStackAfterLastTerminal(TStack *stack) {
    if(!stack)
        return;

    StackElementPtr temp = NULL;   // points to the last terminal
    StackElementPtr prev = NULL; // points to the element before the last terminal
    StackElementPtr newElPtr = malloc(sizeof(struct StackElement));
    temp = stack->firstElement;
    while(temp != NULL) {
        // check if current element's token is terminal
        if(temp->data->type != $E) {
            break; // the last terminal found
        }
        prev = temp;
        temp = temp->nextElement;
    }
    // create shift token and insert it before the active element
    Token *shiftToken = (Token *) malloc(sizeof(Token));
    shiftToken->type = $SHIFT;
    shiftToken->value.str = "<";
    newElPtr->data=shiftToken;

    // insert new element into the stack
    if(prev == NULL)
    {
        PushTStack(stack,shiftToken);
    } else
    {
        prev->nextElement = newElPtr;
        newElPtr->nextElement = temp;
    }



}

void StackFirst(TStack *stack) {
    stack->activeElement = stack->firstElement;
}

Token* TopTStack(TStack *stack) {
    return (stack->firstElement->data);
}

void PopTStack(TStack *stack) {
    if(stack->firstElement != NULL) {
        StackElementPtr tempElPtr = stack->firstElement;

        if(stack->activeElement == stack->firstElement) {
            stack->activeElement = NULL;
        }
        if(stack->firstElement->nextElement == NULL) {
            stack->firstElement = NULL;
        }
        else {
            stack->firstElement = stack->firstElement->nextElement;
        }

        free(tempElPtr);
    }
}



int getIndexOfSymbol(Token *token) {
    switch(token->type) {
        case $ADD: return PLUS_IND;
        case $SUB: return MINUS_IND;
        case $MUL: return MUL_IND;
        case $LPAR: return LEFT_BR_IND;
        case $RPAR: return RIGHT_BR_IND;
        case $VAR:
        case $NUMBER:
        case $INTEGER:
        case $INT:
        case $E:
            return IDENT_IND;
        case $DIV: return DIV_IND;
        case $NEQ: return NOTEQ_IND;
        case $LT: return LT_IND;
        case $GT: return GT_IND;
        case $LTE: return LTE_IND;
        case $GTE: return GTE_IND;
        case $EQ: return EQ_IND;
        case $STR:
        case $STRING:
            return STR_IND;
        default: return DLR_IND;
    }
}

// Find out whether the tokens match any of the precedence rules
precedence_table_rules determineExistingRule(Token *s1, Token *s2, Token *s3) {
    if(s1->type == $E) {
        if(s2->type == $ADD) {
            if(s3->type == $E) {
                return E_PLUS_E;
            }
        }
        if(s2->type == $MUL) {
            if(s3->type == $E) {
                return E_MUL_E;
            }
        }
        if(s2->type == $DIV) {
            if(s3->type == $E) {
                return E_DIV_E;
            }
        }
        if(s2->type == $GT) {
            if(s3->type == $E) {
                return E_GT_E;
            }
        }
        if(s2->type == $GTE) {
            if(s3->type == $E) {
                return E_GEQ_E;
            }
        }
        if(s2->type == $LT) {
            if(s3->type == $E) {
                return E_LT_E;
            }
        }
        if(s2->type == $LTE) {
            if (s3->type == $E) {
                return E_LEQ_E;
            }
        }

        if(s2->type == $EQ) {
            if(s3->type == $E) {
                return E_EQ_E;
            }
        }
        if(s2->type == $NEQ) {
            if(s3->type == $E) {
                return E_NEQ_E;
            }
        }
    }

    else if(s1->type == $LPAR) {
        if(s2->type == $E) {
            if(s3->type == $RPAR) {
                return E_BRACKET;
            }
        }
    }

    else {
        return NOT_FOUND;   // error, rule not found
    }
}

int determinePosOfLastShift(TStack *stack) {
    int pos = 1;
    StackElementPtr temp = stack->firstElement;
    while(temp != NULL) {
        if(temp->data->type == $SHIFT) {    // shift found, return current position
            return pos;
        }
        pos++;
        temp = temp->nextElement;
    }
    return 0; // shift not found
}

void skipWhiteSpaceTokens(Token *tokenPtr) {
    while(GetNextToken(tokenPtr) == TOKEN_OK)
    {
        if(tokenPtr->type != $WS)
            break;
    }
}

Token *getFirstTerminal(TStack *tokenStack) {
    StackElementPtr temp = tokenStack->firstElement;
    while(temp != NULL) {
        if(temp->data->type != $E) {
            break;
        }
        temp = temp->nextElement;
    }
    return temp->data;
}

int makeExpression(Token *obtainedToken, Program *prog) {
    // check arguments
    TStack *tokenStack = (TStack*) malloc(sizeof(TStack));
    InitTStack(tokenStack);

    if(obtainedToken == NULL) {
        fprintf(stderr, "make_expression function was called without token");
    }
    if(tokenStack == NULL) {
        fprintf(stderr, "make_expression function was called without stack");
    }

    // determine column of an input token in precedence table and row of symbol on top of the stack
    int col = getIndexOfSymbol(obtainedToken);
//    printf("obtained token is of type %c\n", obtainedToken->type);
//    printf("col: %d\n", col);
    Token *topOfStack = TopTStack(tokenStack);
    Token *topOfStackReduce = TopTStack(tokenStack);
//    printf("Token on top of the stack is of type %c\n", topStack->type);
    int row = getIndexOfSymbol(topOfStack);
//    printf("row: %d\n", row);

    // TODO look into the precedence table and continue in the algorithm
    char precedenceSym = precedenceTable[row][col];
    while(obtainedToken->type != $LEX_ERR) {   // indicates lexical error (there are no more characters)
        switch(precedenceSym) {
            case '=':   // push(b) & precti dalsi symbol b ze vstupu
                PushTStack(tokenStack, obtainedToken);
                skipWhiteSpaceTokens(obtainedToken);
                if(obtainedToken->type == $LEX_ERR) // indicates lexical error (there are no more characters)
                    break;
                col = getIndexOfSymbol(obtainedToken);  // get current column number
                topOfStack = getFirstTerminal(tokenStack); // get token that is on top of the stack
                row = getIndexOfSymbol(topOfStack); // get current row number
                precedenceSym = precedenceTable[row][col];
                continue;
            case '<':   // zamen a za a< na zasobiku & push(b) & precti dalsi symbol b ze vstupu
                PushTStackAfterLastTerminal(tokenStack); // zamen a za a<
                PushTStack(tokenStack, obtainedToken); // push b
                skipWhiteSpaceTokens(obtainedToken);  // precti symbol b ze vstupu
                if(obtainedToken->type == $LEX_ERR)
                    break;

                col = getIndexOfSymbol(obtainedToken);  // get current column number
                topOfStack = getFirstTerminal(tokenStack); // get token that is on top of the stack
                row = getIndexOfSymbol(topOfStack); // get current row number
                precedenceSym = precedenceTable[row][col];
                continue;
            case '>': // pokud <y je na vrcholu zasobniku and r: A->y ∈ P, zamen <y za A
                // 1) muze byt E -> E operator E
                // pokud to nevrati error, tak vypopuju ty 3 posledni elementy a pushnu tam to E
                // a necham vygenerovat ten operator
                if(determinePosOfLastShift(tokenStack) == 4) {
                    Token* arg1 = tokenStack->firstElement->data;
                    Token* arg2 = tokenStack->firstElement->nextElement->data;
                    Token* arg3 = tokenStack->firstElement->nextElement->nextElement->data;

                    precedence_table_rules ruleFound = determineExistingRule(arg1, arg2, arg3);
                    if(ruleFound != NOT_FOUND) {
                        PopTStack(tokenStack);
                        GenerateOperand(prog, tokenStack->firstElement->data->type); // vygeneruje operator
                        PopTStack(tokenStack);
                        PopTStack(tokenStack);
                        PopTStack(tokenStack); // je potreba popnout i ten shift operator
                        Token *newToken = (Token*) malloc(sizeof(Token));
                        newToken->type = $E;
                        newToken->value.str = "E";
                        PushTStack(tokenStack, newToken);
                    } else {
                        // TODO
                    }
                }

                // 2) muze byt pouze E -> i
                // pokud je to pouze identifikator, tak ho vygeneruju
                else if(determinePosOfLastShift(tokenStack) == 2) {
                    if(tokenStack->firstElement->data->type == $VAR || tokenStack->firstElement->data->type == $STR || tokenStack->firstElement->data->type == $NUMBER || tokenStack->firstElement->data->type == $INTEGER) {
                        Token *newToken = (Token*) malloc(sizeof(Token));

                        //Generate token
                        memcpy(newToken, &prog->token, sizeof(Token));
                        memcpy(&prog->token,tokenStack->firstElement->data, sizeof(Token));
                        GeneratePush(prog);
                        // FIXME GeneratePush
                        memcpy(&prog->token,newToken, sizeof(Token));

                        PopTStack(tokenStack);
                        PopTStack(tokenStack);
                        newToken->type = $E;
                        newToken->value.str = "E";
                        PushTStack(tokenStack, newToken);
                    }
                }

                col = getIndexOfSymbol(obtainedToken);  // get current column number
                topOfStack = getFirstTerminal(tokenStack); // get token that is on top of the stack
                row = getIndexOfSymbol(topOfStack); // get current row number
                precedenceSym = precedenceTable[row][col];
                continue;
            case 'E':
                if(col == DLR_IND && row == DLR_IND) {
                    GenerateExprResult(prog,"$result",prog->active_frame);
                    return SYNTAX_OK;
                }
                return SYNTAX_ERR;
        }
    }

    if(precedenceSym == 'E') {
        fprintf(stderr, "Syntax analyz error");
        return SYNTAX_ERR;
    }
}

int testExpr() {
//    Token* testToken;
//    testToken = (Token *) malloc(sizeof(Token));
//    testToken->type = $LPAR;
//    int indexOfLpar = getIndexOfSymbol(testToken);

    //Token* testRowToken;
    //testRowToken = (Token *) malloc(sizeof(Token));
    //testRowToken->type = $EQ;


    //TStack *testStack;
    //testStack = (TStack *) malloc(sizeof(TStack));
//    InitTStack(testStack);
//    PushTStack(testStack, testRowToken);
//    Token* getToken;
//    getToken = TopTStack(testStack);
//    int rowIndex = getIndexOfSymbol(getToken);
//
//    printf("Index of LPAAAAAAAAAARRRRRRR %d\n", indexOfLpar);
//    printf("Index of row %d\n", rowIndex);
//
//    free(testToken);
//    free(testStack);
//    free(testRowToken);
}

