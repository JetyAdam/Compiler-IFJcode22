//
// Created by Lukáš on 30.09.2022.
//
#include "global_def.h"

#ifndef IFJ22_LEX_ANALYZ_H
#define IFJ22_LEX_ANALYZ_H
typedef enum{

    State_Start,
    FuncId,
    VarIdStart,
    StringStart,
    StringEnd,
    VarIdEnd,
    LeftPar,
    RightPar,
    LeftCurBr,
    RightCurBr,
    Add,
    Sub,
    Mul,
    Div,
    Div2,
    LT,
    LTQ,
    GT,
    GTQ,
    Equal1,
    Oeq,
    Equal2,
    Equal3,
    NotEqual1,
    NotEqual2,
    NotEqual3,
    Semicolon,
    LineCom1,
    LineCom2,
    BlockCom1,
    BlockCom2,
    BlockCom3,
    WSpace,
    StartTok1,
    StartTok2,
    StartTok3,
    StartTokEnd,
    Colon,
    Comma,
    QMark,
    Number,
    EscapeSeq,
    floatNumbers,
    floatNumbersSwitch,
    floatNumbersExp,
    floatNumbersExpMark,
    ErrorState

} States;

typedef struct tqelem{
    char data;
    struct tqelem *nextPtr;
}TQElem;

typedef struct tqueue{
    TQElem *beg;
    TQElem *end;
}TQueue;

/**
 * Get next token from file
 * @param Tok initialized token
 * @return TOKEN_OK or Errcode from global_def
 */
int GetNextToken(Token *Tok);
void SetInputFile(FILE *f);

void Clear_char_stack();

#endif //IFJ22_LEX_ANALYZ_H
