//
// Created by Lukáš on 30.09.2022.
//

#include "lex_analyz.h"
#include "global_def.h"
#include "SmallMC.h"
#include <ctype.h>
#include <string.h>

FILE *Source;



Stack charStack;

int stackC(FILE *f){
    if(charStack.top > 0)
    {
        int c = (int)Stack_pop(&charStack);
        return c;
    }else
    {
        return getc(f);
    }
}

void stackUnget(int c,FILE *f) {
    Stack_push(&charStack,(void*)c);
}

#define getc(F) stackC(F)
#define ungetc(C,F) stackUnget(C,F)

void InitQueue (TQueue *q){
    q->beg = NULL;
    q->end = NULL;
}

bool IsEmpty (TQueue *q){
    return (q->beg == NULL);
}

void AddSymbol (TQueue *q, char data){
    TQElem *newElemPtr = (TQElem *)malloc(sizeof(TQElem));
    if (newElemPtr == NULL){
        return;
        //TODO
    }
    newElemPtr->data = data; //naplnění nového prvku
    newElemPtr->nextPtr = NULL; //ukončení nového prvku
    if (q->beg == NULL){ // fronta je prázdná,
        q->beg = newElemPtr;//vlož nový jako první a jediný
    }
    else {//obsahuje nejméně jeden prvek, přidej na konec
        q->end->nextPtr = newElemPtr;
    }
    q->end = newElemPtr; // korekce konce fronty
}

void Remove (TQueue *q)
{
    if (q->beg != NULL){ //Fronta je neprázdná
        TQElem *elemPtr = q->beg;
        if (q->beg == q->end){
            q->end = NULL; //Zrušil se poslední a jediný
        }
        q->beg = q->beg->nextPtr;
        free(elemPtr);
    }
}

char Front (TQueue *q)
{
    return (q->beg->data);
}

char Last (TQueue *q)
{
    return (q->end->data);
}

char *GetString (TQueue *data, int charCounter){
    char *string = malloc(charCounter+1);
    int i = 0;
    while (i != charCounter){
        if (!IsEmpty(data)) {
            string[i] = Front(data);
            Remove(data);
        }
        i++;
    }
    string[i] = '\000';
    return string;
}
// ------------------------------ Funkce pro zjisteni, zda a o jake klicove slovo se jedna ------------------------------ //
int IsKeyWord(TQueue *data, int charCounter) {

    int i = 0;
    char kw[charCounter+1];
    while (i != charCounter){
        if (!IsEmpty(data)){
            kw[i] = Front(data);
            i++;
        }
    }
    kw[i] = '\000';
    if (strcmp(kw, "else") == 0){
        return 1;
    }else if (strcmp(kw, "float") == 0) {
        return 2;
    }else if (strcmp(kw, "function") == 0){
        return 3;
    }else if (strcmp(kw, "if") == 0){
        return 4;
    }else if (strcmp(kw, "int") == 0){
        return 5;
    }else if (strcmp(kw, "null") == 0){
        return 6;
    }else if (strcmp(kw, "return") == 0){
        return 7;
    }else if (strcmp(kw, "string") == 0){
        return 8;
    }else if (strcmp(kw, "void") == 0){
        return 9;
    }else if (strcmp(kw, "while") == 0){
        return 10;
    }else{
        return 11;
    }
}

void ClearStruct(TQueue *data, int charCounter){
    TQElem *i = data->beg;
    while (i != NULL){
        i = i->nextPtr;
        Remove(data);
    }
}

void Clear_char_stack()
{
    Stack_free(&charStack);
}

void SetInputFile(FILE *f)
{
    Stack_init(&charStack);
    Source = f;
}

int row = 1;    // Pozice charakteru v ramci 2D pole (radek)
int column = 1; // Pozice nacteneho charakteru v ramci 2D pole (sloupec)

int GetNextToken(Token *Tok)
{

    switch (Tok->type){
        case $FID:
        case $STR:
        case $VAR:
        case $ELSE:
        case $FLOAT:
        case $FUNCTION:
        case $IF:
        case $INT:
        case $NULL:
        case $RETURN:
        case $STRING:
        case $VOID:
        case $WHILE:
            if (Tok->value.str != NULL){
                free(Tok->value.str);
                Tok->value.str = NULL;
            }
            break;
        default:
            Tok->value.str = NULL;
    }


    TQueue FIdQueue;
    int charCounter = 0;
    int EscapeSwitch = 0;
    int character;  // Nacitany charakter
    int state = State_Start;
    char string[0];
    InitQueue(&FIdQueue);

    while (true)
    {

        column++;

        //character = getc(Source);
        character = getc(stdin);

        switch (state)
        {

            case State_Start:
                if (character == EOF){
                    Tok->type = $EOF;
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    ungetc(character, Source);
                    return 0;
                }
                if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character == '_'))
                {
                    state = FuncId;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
                if (character >= '0' && character <= '9'){
                    state = Number;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
                if (character == '$')
                {
                    state = VarIdStart;
                    column --;
                    continue;
                }
                if (character == '\"')
                {
                    state = StringStart;
                    column --;
                    continue;
                }
                if (character == '(')
                {
                    state = LeftPar;
                    column --;
                    continue;
                }
                if (character == ')')
                {
                    state = RightPar;
                    column --;
                    continue;
                }
                if (character == '{')
                {
                    state = LeftCurBr;
                    column --;
                    continue;
                }
                if (character == '}')
                {
                    state = RightCurBr;
                    column --;
                    continue;
                }
                if (character == '+')
                {
                    state = Add;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
                if (character == '-')
                {
                    state = Sub;
                    column --;
                    continue;
                }
                if (character == '*')
                {
                    state = Mul;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
                if (character == '/')
                {
                    state = Div;
                    column --;
                    continue;
                }
                if (character == '<')
                {
                    state = LT;
                    column --;
                    continue;
                }
                if (character == '>')
                {
                    state = GT;
                    column --;
                    continue;
                }
                if (character == '=')
                {
                    state = Equal1;
                    column --;
                    continue;
                }
                if (character == '!')
                {
                    state = NotEqual1;
                    column --;
                    continue;
                }
                if (character == ';')
                {
                    state = Semicolon;
                    column --;
                    continue;
                }
                if (character == ':'){
                    state = Colon;
                    column --;
                    continue;
                }
                if (character == ','){
                    state = Comma;
                    column --;
                    continue;
                }
                if (character == '?'){
                    state = QMark;
                    column --;
                    continue;
                }
                if (isspace(character))
                {
                    if (character == '\n')
                    {
                        row++;
                        column = 1;
                    }
                    state = WSpace;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
                else
                {
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case FuncId:
                if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character == '_')){
                    charCounter ++;
                    AddSymbol(&FIdQueue, character);
                    continue;
                }else{
                    char *FuncId = GetString(&FIdQueue, charCounter);
                    if (character != EOF){
                        column --;
                    }
                    //int KeyWord = IsKeyWord(&FIdQueue, charCounter);
                    if (strcmp(FuncId, "else") == 0) { // KeyWord "else"
                        Tok->type = $ELSE;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "float") == 0) { // KeyWord "float"
                        Tok->type = $FLOAT;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "function") == 0) { // KeyWord "function"
                        Tok->type = $FUNCTION;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "if") == 0) { // KeyWord "if"
                        Tok->type = $IF;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "int") == 0) { // KeyWord "int"
                        Tok->type = $INT;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "null") == 0) { // KeyWord "null"
                        Tok->type = $NULL;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "return") == 0) { // KeyWord "return"
                        Tok->type = $RETURN;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "string") == 0) { // KeyWord "string"
                        Tok->type = $STRING;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "void") == 0) { // KeyWord "void"
                        Tok->type = $VOID;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else if (strcmp(FuncId, "while") == 0) { // KeyWord "while"
                        Tok->type = $WHILE;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        ClearStruct(&FIdQueue, charCounter);
                        return 0;
                    }else { // Not KeyWord
                        Tok->type = $FID;
                        Tok->value.str = FuncId;
                        Tok->loc.character = column;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        return 0;
                    }
                }
            case VarIdStart:
                if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character == '_'))
                {
                    charCounter ++;
                    AddSymbol(&FIdQueue, '$');
                    column --;
                    state = VarIdEnd;
                    ungetc(character, Source);
                    continue;
                }
                else
                {
                    column --;
                    state = ErrorState;
                }
                ungetc(character, Source);
                continue;
            case VarIdEnd:
                if ((character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character >= 0 && character <= 9) || (character == '_'))
                {
                    charCounter ++;
                    AddSymbol(&FIdQueue, character);
                    continue;
                }
                else if (isspace(character) || character == ';' || character == ')' || character == EOF ||
                           character == '+' || character == '-' || character == '*' || character == '/' ||
                           character == ',')
                {
                    char *VarId = GetString(&FIdQueue, charCounter);
                    Tok->type = $VAR;
                    Tok->value.str = VarId;
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    ungetc(character, Source);
                    return 0;
                }
                else
                {
                    char *VarId = GetString(&FIdQueue, charCounter);
                    Tok->value.str = VarId;
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                }
                continue;
            case StringStart:
                if (character == '\"')
                {
                    state = StringEnd;
                    continue;
                }else if (character == '\n') {
                    charCounter ++;
                    AddSymbol(&FIdQueue, '\\');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '1');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    continue;
                }else if (character == '\t') {
                    charCounter ++;
                    AddSymbol(&FIdQueue, '\\');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '1');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '1');
                    continue;
                }else if (character == ' ') {
                    charCounter ++;
                    AddSymbol(&FIdQueue, '\\');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '3');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '2');
                    continue;
                }else if (character == EOF) {
                    state = ErrorState;
                    column --;
                    continue;
                }else if (character == '\\'){
                    state = EscapeSeq;
                    column --;
                    continue;
                }else{
                    charCounter ++;
                    AddSymbol(&FIdQueue, character);
                    continue;
                }
            case EscapeSeq:
                if (character >= '0' && character <= '9'){
                    if (EscapeSwitch == 0 && character <= '2') {
                        charCounter++;
                        AddSymbol(&FIdQueue, '\\');
                        charCounter++;
                        AddSymbol(&FIdQueue, character);
                        EscapeSwitch++;
                        continue;
                    }else if (EscapeSwitch == 1){
                        if (Last(&FIdQueue) == '2'){
                            if (character <= '5'){
                                charCounter++;
                                AddSymbol(&FIdQueue, character);
                                EscapeSwitch++;
                                continue;
                            }else{
                                state = ErrorState;
                                column = column - EscapeSwitch;
                                continue;
                            }
                        }else{
                            charCounter++;
                            AddSymbol(&FIdQueue, character);
                            EscapeSwitch++;
                            continue;
                        }
                    }else if (EscapeSwitch == 2) {
                        if (Last(&FIdQueue) == '5') {
                            if (character <= '5') {
                                charCounter++;
                                AddSymbol(&FIdQueue, character);
                                EscapeSwitch = 0;
                                state = StringStart;
                                continue;
                            } else {
                                state = ErrorState;
                                column = column - EscapeSwitch;
                                continue;
                            }
                        } else {
                            charCounter++;
                            AddSymbol(&FIdQueue, character);
                            EscapeSwitch = 0;
                            state = StringStart;
                            continue;
                        }
                    }else if (EscapeSwitch > 2){
                        state = StringStart;
                        EscapeSwitch = 0;
                        column --;
                        ungetc(character, Source);
                        continue;
                    }else{
                        state = ErrorState;
                        column = column - EscapeSwitch;
                        continue;
                    }
                }else if (character == 'n'){
                    charCounter ++;
                    AddSymbol(&FIdQueue, '\\');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '1');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    state = StringStart;
                    continue;
                }else if (character == 't'){
                    charCounter ++;
                    AddSymbol(&FIdQueue, '\\');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '1');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '1');
                    state = StringStart;
                    continue;
                }
                else if (character == '$'){
                    charCounter ++;
                    AddSymbol(&FIdQueue, '\\');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '3');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '6');
                    state = StringStart;
                    continue;
                }
                else if (character == '"'){
                    charCounter ++;
                    AddSymbol(&FIdQueue, '\\');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '3');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '4');
                    state = StringStart;
                    continue;
                }
                else if (character == '\\'){
                    charCounter ++;
                    AddSymbol(&FIdQueue, '\\');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '0');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '9');
                    charCounter ++;
                    AddSymbol(&FIdQueue, '2');
                    state = StringStart;
                    continue;
                }else{
                    state = ErrorState;
                    column -= 2;
                    continue;
                }
            case StringEnd:
                    if(character == EOF){
                        char *StringValue = GetString(&FIdQueue, charCounter);
                        Tok->type = $STR;
                        Tok->value.str = StringValue;
                        Tok->loc.character = column-1;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        return 0;
                    }else{
                        char *StringValue = GetString(&FIdQueue, charCounter);
                        Tok->type = $STR;
                        Tok->value.str = StringValue;
                        Tok->loc.character = column-1;
                        Tok->loc.line = row;
                        ungetc(character, Source);
                        return 0;
                    }
            case LeftPar:
                Tok->type = $LPAR;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case RightPar:
                Tok->type = $RPAR;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case LeftCurBr:
                Tok->type = $LCURL;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case RightCurBr:
                Tok->type = $RCURL;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case Add:
                Tok->type = $ADD;
                Tok->loc.character = column;
                Tok->loc.line = row;
                //ungetc(character, Source);
                return 0;
            case Sub:
                if (character >= '1' && character <= '9'){
                    charCounter ++;
                    AddSymbol(&FIdQueue, '-');
                    ungetc(character, Source);
                    state = Number;
                    continue;
                }else {
                    Tok->type = $SUB;
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    ungetc(character, Source);
                    return 0;
                }
            case Mul:
                Tok->type = $MUL;
                Tok->loc.character = column;
                Tok->loc.line = row;
                //ungetc(character, Source);
                return 0;
            case Div:
                if (character == '/')
                {
                    state = LineCom1;
                    continue;
                }
                else if (character == '*')
                {
                    state = BlockCom1;
                    column --;
                    continue;
                }
                else
                {
                    state = Div2;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case LT:
                if (character == '=')
                {
                    state = LTQ;
                    column --;
                    continue;
                }
                else if (character == '?'){
                    state = StartTok1;
                    column --;
                    continue;
                }
                else
                {
                    Tok->type = $LT;
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    ungetc(character, Source);
                    return 0;
                }
            case LTQ:
                Tok->type = $LTE;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case GT:
                if (character == '=')
                {
                    state = GTQ;
                    column --;
                    continue;
                }
                else
                {
                    Tok->type = $GT;
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    ungetc(character, Source);
                    return 0;
                }
            case GTQ:
                Tok->type = $GTE;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case Equal1:
                if (character == '=')
                {
                    state = Equal2;
                    column --;
                    continue;
                }
                else
                {
                    state = Oeq;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case Oeq:
                Tok->type = $OEQ;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case Equal2:
                if (character == '=')
                {
                    state = Equal3;
                    column --;
                    continue;
                }
                else
                {
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case Equal3:
                Tok->type = $EQ;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case NotEqual1:
                if (character == '=')
                {
                    state = NotEqual2;
                    column --;
                    continue;
                }
                else
                {
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case NotEqual2:
                if (character == '=')
                {
                    state = NotEqual3;
                    column --;
                    continue;
                }
                else
                {
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case NotEqual3:
                Tok->type = $NEQ;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case Semicolon:
                Tok->type = $SEMICOL;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case LineCom1:
                if (character == '\n' || character == EOF)
                {
                    column = 0;
                    row++;
                    state = LineCom2;
                    continue;
                }
                else
                {
                    continue;
                }
            case LineCom2:
                Tok->type = $WS;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case BlockCom1:
                if (character == '*')
                {
                    state = BlockCom2;
                    column --;
                    continue;
                }
                else
                {
                    continue;
                }
            case BlockCom2:
                if (character == '/')
                {
                    state = BlockCom3;
                    column --;
                    continue;
                }
                else
                {
                    state = BlockCom2;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case BlockCom3:
                Tok->type = $WS;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case Div2:
                Tok->type = $DIV;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case WSpace:
                Tok->type = $WS;
                Tok->loc.character = column;
                Tok->loc.line = row;
                //No Ungetc
                return 0;
            case StartTok1:
                if(character == 'p') {
                    state = StartTok2;
                    column--;
                    continue;
                }else{
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case StartTok2:
                if(character == 'h') {
                    state = StartTok3;
                    column--;
                    continue;
                }else{
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case StartTok3:
                if(character == 'p') {
                    state = StartTokEnd;
                    column--;
                    continue;
                }else{
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case StartTokEnd:
                if (isspace(character)){
                    Tok->type = $START;
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    ungetc(character, Source);
                    return 0;
                }else{
                    state = ErrorState;
                    column --;
                    ungetc(character, Source);
                    continue;
                }
            case Colon:
                Tok->type = $COLON;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case Comma:
                Tok->type = $COMMA;
                Tok->loc.character = column;
                Tok->loc.line = row;
                ungetc(character, Source);
                return 0;
            case QMark:
                if (character == '>'){
                    Tok->type = $EOF;
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    ungetc(character, Source);
                    return 0;
                }else
                    Tok->type = $QMARK;
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    ungetc(character, Source);
                    return 0;
            case Number:
                if (character >= '0' && character <= '9'){
                    charCounter ++;
                    AddSymbol(&FIdQueue, character);
                    continue;
                }else if (isspace(character) || character == ';' || character == ')' || character == EOF ||
                            character == '+' || character == '-' || character == '*' || character == '/' ||
                            character == ',') {
                    char *IntStr = GetString(&FIdQueue, charCounter);
                    Tok->type = $INTEGER;
                    Tok->value.i = atoi(IntStr);
                    free(IntStr);
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    column--;
                    ungetc(character, Source);
                    return 0;
                }else if (character == '.' || character == 'e' || character == 'E'){
                    state = floatNumbersSwitch;
                    ungetc(character, Source);
                    continue;
                }else{
                    state = ErrorState;
                    ungetc(character, Source);
                    continue;
                }
            case floatNumbersSwitch:
                if (character == '.'){
                    charCounter ++;
                    AddSymbol(&FIdQueue, character);
                    state = floatNumbers;
                    continue;
                }else if(character == 'e' || character == 'E'){
                    charCounter++;
                    AddSymbol(&FIdQueue, 'e');
                    state = floatNumbersExpMark;
                    continue;
                }else{
                    state = ErrorState;
                    ungetc(character, Source);
                    continue;
                }
            case floatNumbers:
                if (character >= '0' && character <= '9') {
                    charCounter++;
                    AddSymbol(&FIdQueue, character);
                    continue;
                }else if (character == 'e' || character == 'E'){
                    charCounter++;
                    AddSymbol(&FIdQueue, 'e');
                    state = floatNumbersExpMark;
                    continue;
                }else if (isspace(character) || character == ';' || character == ')' || character == EOF ||
                          character == '+' || character == '-' || character == '*' || character == '/' ||
                          character == ',') {
                    char *FloatStr = GetString(&FIdQueue, charCounter);
                    Tok->type = $NUMBER;
                    Tok->value.f = strtod(FloatStr, NULL);
                    free(FloatStr);
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    column--;
                    ungetc(character, Source);
                    return 0;
                }else{
                    state = ErrorState;
                    ungetc(character, Source);
                    continue;
                }
            case floatNumbersExpMark:
                if (character == '+'){
                    state = floatNumbersExp;
                    continue;
                }else if (character == '-'){
                    charCounter++;
                    AddSymbol(&FIdQueue, character);
                    state = floatNumbersExp;
                    continue;
                }else{
                    state = ErrorState;
                    ungetc(character, Source);
                    continue;
                }
            case floatNumbersExp:
                if (character >= '0' && character <= '9') {
                    charCounter++;
                    AddSymbol(&FIdQueue, character);
                    continue;
                }else if (isspace(character) || character == ';' || character == ')' || character == EOF ||
                          character == '+' || character == '-' || character == '*' || character == '/' ||
                          character == ',') {
                    char *FloatStr = GetString(&FIdQueue, charCounter);
                    Tok->type = $NUMBER;
                    Tok->value.f = strtod(FloatStr, NULL);
                    free(FloatStr);
                    Tok->loc.character = column;
                    Tok->loc.line = row;
                    column--;
                    ungetc(character, Source);
                    return 0;
                }else{
                    state = ErrorState;
                    ungetc(character, Source);
                    continue;
                }
            case ErrorState:
                Tok->type = LEX_ERR;
                Tok->loc.character = column;
                Tok->loc.line = row;
                return LEX_ERR;
            default:
                return LEX_ERR;
        }
    }
    return 0;
}
