//
// Created by xletak00 on 05.10.2022.
//
#include "global_def.h"
#include <assert.h>
#include <stdbool.h>
#include "SmallMC.h"

///Token name LUT
const char* Tok_names[] = {
        "$WS",
        "$EOF",
        "$START",
        //-------Keyword tokens--------
        "$ELSE",
        "$FLOAT",
        "$FUNCTION",
        "$IF",
        "$INT",
        "$NULL",
        "$RETURN",
        "$STRING",
        "$VOID",
        "$WHILE",
        //--------Variables and constants------
        "$FID",
        "$VAR",
        "$STR",
        "$NUMBER",
        "$INTEGER",
        "$EXPR",
        //--------Operators----------
        "$MUL",
        "$DIV",
        "$ADD",
        "$SUB",
        "$DOT",
        "$OEQ",
        //-------Logical operators------
        "$LT",
        "$GT",
        "$LTE",
        "$GTE",
        "$EQ",
        "$NEQ",
        //-------Symbols-----------
        "$COLON",
        "$COMMA",
        "$SEMICOL",
        "$LPAR",
        "$RPAR",
        "$LCURL",
        "$RCURL",
        "$QMARK"
};

///Instruction name LUT {Instruction_name, arg_count}
const char* Instruction_names[][2] = {
     {"MOVE","2"},
     {"CREATEFRAME","0"},
     {"PUSHFRAME","0"},
     {"POPFRAME","0"},
     {"DEFVAR","1"},
     {"CALL","1"},
     {"RETURN","0"},
     {"PUSHS","1"},
     {"POPS","1"},
     {"CLEARS","0"},
    {"ADD","3"},
    {"SUB","3"},
    {"MUL","3"},
    {"DIV","3"},
    {"IDIV","3"},
    {"ADDS","0"},
    {"SUBS","0"},
    {"MULS","0"},
    {"DIVS","0"},
    {"IDIVS","0"},
    {"LT","3"},
    {"GT","3"},
    {"EQ","3"},
    {"LTS","0"},
    {"GTS","0"},
    {"EQS","0"},
    {"AND","3"},
    {"OR","3"},
    {"NOT","3"},
    {"ANDS","0"},
    {"ORS","0"},
    {"NOTS","0"},
    {"INT2FLOAT","2"},
    {"FLOAT2INT","2"},
    {"INT2CHAR","2"},
    {"STRI2INT","3"},
    {"INT2FLOATS","0"},
    {"FLOAT2INTS","0"},
    {"INT2CHARS","0"},
    {"STRI2INTS","0"},
    {"READ","2"},
    {"WRITE","1"},
    {"CONCAT","3"},
    {"STRLEN","2"},
    {"GETCHAR","3"},
    {"SETCHAR","3"},
    {"TYPE","2"},
    {"LABEL","1"},
    {"JUMP","1"},
    {"JUMPIFEQ","3"},
    {"JUMPIFNEQ","3"},
    {"JUMPIFEQS","1"},
    {"JUMPIFNEQS","1"},
    {"EXIT","1"},
    {"BREAK","0"},
    {"DPRINT","1"},
    {"CCOMMENT","1"}
};

const char* Value_names[] = {
        "Value_string",
        "Value_int",
        "Value_float",
        "Value_void",
        "Value_any",
        "Value_function"
};

void Dprintf_location(Location loc)
{
    fprintf(stderr,"Location{\n");
    fprintf(stderr,"    File: %s\n",loc.file);
    fprintf(stderr,"    Line: %d\n",loc.line);
    fprintf(stderr,"    Char: %d\n",loc.character);
    fprintf(stderr,"}\n");
}

void Dprintf_token(Token tok) {
    fprintf(stderr,"Token{\n");
    fprintf(stderr,"    Location{\n");
    fprintf(stderr,"        File: %s\n",tok.loc.file);
    fprintf(stderr,"        Line: %d\n",tok.loc.line);
    fprintf(stderr,"        Char: %d\n",tok.loc.character);
    fprintf(stderr,"    }\n");
    fprintf(stderr,"    Type: %s\n",Tok_names[tok.type]);
    fprintf(stderr,"    Value{\n");
    switch(tok.type)
    {
        case $VAR:
        case $FID:
        case $STR:
            fprintf(stderr,"        str: %s\n",tok.value.str);
        default:
            fprintf(stderr,"        f: %f\n",tok.value.f);
            fprintf(stderr,"        i: %d\n",tok.value.i);
    }
    fprintf(stderr,"    }\n");
    fprintf(stderr,"}\n");
}

void printf_location(Location loc) {
    fprintf(stderr,"%s:%d:%d: ",loc.file,loc.line,loc.character);
}

void Dprintf_Stable_data(Stable_data data) {
    fprintf(stderr, "Stable_data{\n");
    fprintf(stderr, "    Location{\n");
    fprintf(stderr, "        File: %s\n", data.defined.file);
    fprintf(stderr, "        Line: %d\n", data.defined.line);
    fprintf(stderr, "        Char: %d\n", data.defined.character);
    fprintf(stderr, "    }\n");
    fprintf(stderr, "    Value_type: %s", Value_names[data.type]);
    fprintf(stderr, "    Used: %s", data.used ? "true" : "false");
    fprintf(stderr, "    Value{\n");
    switch (data.type) {
        case Value_float:
            fprintf(stderr, "        f: %f\n", data.val.f);
            break;
        case Value_int:
            fprintf(stderr, "        i: %d\n", data.val.i);
            break;
        case Value_string:
            fprintf(stderr, "        str: %s\n", data.val.str);
            break;
        case Value_function:
            fprintf(stderr, "        function{\n");
            fprintf(stderr, "            param_count: %d\n", data.val.func->param_count);
            for(int i = 0; i < data.val.func->param_count; i++)
            {
                fprintf(stderr, "              param_name: %s\n", data.val.func->param_names[i]);
                fprintf(stderr, "              param_type: %s\n", Value_names[data.val.func->param_types[i]]);
            }
            fprintf(stderr, "            return_type: %s\n", Value_names[data.val.func->return_type]);
            fprintf(stderr, "        }\n");
            break;
        default:
            ERROR_MSG(INTERNAL_ERR, "Value provided is not valid");
            break;
    }
    fprintf(stderr,"    }\n");
    fprintf(stderr,"}\n");
}

unsigned encode_arg_types(Iarg_types arg_types) {
    return (0 | (arg_types.arg1 & 7) | ((arg_types.arg2 & 7)<<3) | ((arg_types.arg3 & 7)<<6));
}

void decode_arg_types(unsigned encoded, Iarg_types *arg_types) {
    arg_types->arg1 = (encoded & 7);
    arg_types->arg2 = ((encoded>>3) & 7);
    arg_types->arg3 = ((encoded>>6) & 7);
}

void Stack_init(Stack *S){
    S->max = 10;
    S->Stack = malloc(S->max*sizeof(void*));
    S->top = 0;
}

void Stack_free(Stack *S){
    S->max = 0;
    free(S->Stack);
    S->top = 0;
}

void Stack_push(Stack *S,void *item){
    if(S->top==S->max)
    {
        S->max *= 2;
        S->Stack = malloc(S->max*sizeof(void*));
    }
    S->Stack[S->top] = item;
    S->top++;
}

void* Stack_top(Stack *S){
    if(S->top>0)
    {
        return S->Stack[S->top-1];
    }
    ERROR_MSG(99,"Stack underflow");
}

void* Stack_pop(Stack *S){
    if(S->top>0)
    {
        return S->Stack[--S->top];
    }
    ERROR_MSG(99,"Stack underflow");
}
