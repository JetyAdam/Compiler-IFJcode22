//
// Created by Lukáš on 30.09.2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "global_def.h"

#ifndef IFJ22_EXPR_H
#define IFJ22_EXPR_H
#define PRECEDENCE_TABLE_SIZE 16
// Helper constants containing indexes of symbols in precedence table
#define PLUS_IND 0
#define MINUS_IND 1
#define MUL_IND 2
#define LEFT_BR_IND 3
#define RIGHT_BR_IND 4
#define IDENT_IND 5
#define DLR_IND 6
#define DIV_IND 7
#define NOTEQ_IND 8
#define LT_IND 9
#define GT_IND 10
#define LTE_IND 11
#define GTE_IND 12
#define EQ_IND 13
#define STR_IND 14

typedef enum {
    R,   // > Reduce
    S,   // < Shift
    E,   // = Equal
    T   // Error
} precedence_table_operations;

typedef enum {
    E_PLUS_E,   // E -> E+E
    E_MUL_E,    // E -> E*E
    E_DIV_E,    // E -> E/E
    E_GT_E,     // E -> E>E
    E_LT_E,     // E -> E<E
    E_LEQ_E,    // E -> E<=E
    E_GEQ_E,    // E -> E>=E
    E_EQ_E,     // E -> E===E
    E_NEQ_E,    // E -> E!==E
    E_BRACKET,  // E -> (E)
    ID,         // E -> i
    NOT_FOUND   // error, rule not found
} precedence_table_rules;

typedef enum {
    PLUS_MINUS, // +, -
    MUL_DIV,    // *, /
    LEFT_PAR,   // (
    RIGHT_PAR,  // )
    RELATIONAL_OPER,    // >, <, <=, >=, =
    IDENTIF,    // ID
    DOLLAR      // $
} precedence_table_signs;


/** Prvek jednosměrně vázaného seznamu. */
typedef struct StackElement
{
    Token *data;
    struct StackElement *nextElement;
} * StackElementPtr;

/** Jednosměrně vázaný seznam. */
typedef struct
{
    StackElementPtr firstElement;
    StackElementPtr activeElement;
} TStack;

//typedef struct tselem
//{
//    Token *data;
//    struct tselem *nextPtr;
//}TSElem;
//
//typedef struct tstack
//{
//    TSElem *topPtr;
//}TStack;


void InitTStack(TStack *stack);
void DisposeTStack(TStack *stack);
void PushTStack(TStack *stack, Token *sign);
void StackFirst(TStack *stack);
Token* TopTStack(TStack *stack);
void PopTStack(TStack *stack);
void PushTStackAfterLastTerminal(TStack *stack);
int getIndexOfSymbol(Token *token);
precedence_table_rules determineExistingRule(Token *s1, Token *s2, Token *s3);
int makeExpression(Token *obtainedToken, Program *prog);
void displayTStack(TStack *stack);
int determinePosOfLastShift(TStack *stack);
void skipWhiteSpaceTokens(Token *tokenPtr);
int testExpr();
//void InitTStack (TStack *s);
//void PushTStack (TStack *s, Token *sign);
//void PopTStack (TStack *s);
//Token* TopTStack (TStack *s);


#endif //IFJ22_EXPR_H
