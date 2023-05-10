//
// Created by xletak00 on 12.08.2022.
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
//#include "SmallMC.h"

#ifndef IFJ22_GLOBAL_DEF_H
#define IFJ22_GLOBAL_DEF_H

//Turn on memcheck
#define MC_ON

///Useful function for messaging unimplemented features
#define UNIMPLEMENTED(X) do{fprintf(stderr,"%s:%d: %s() Is not fully implemented. missing: %s",__FILE__,__LINE__,__func__,X); exit(100); }while(0)
///Prints error message and exits with specified error code
#define ERROR_MSG(Errcode,msg) do{fprintf(stderr,"%s:%d: %s\n EXITING with %d\n",__FILE__,__LINE__,msg,Errcode); FreeAll(1); exit(Errcode); }while(0)

#define TOKEN_OK 0
#define SYNTAX_OK 0
#define SYMBOL_OK 0
#define BUILDIN_OK 0
#define GENERATED_OK 0
#define LEX_ERR 1
#define SYNTAX_ERR 2
#define SEM_ERR_REDEFINED_FUNC 3
#define SEM_ERR_UNDEFINED_FUNC 3
#define SEM_ERR_FUNC_PAR_COUNT 4
#define SEM_ERR_FUNC_PAR_TYPE  4
#define SEM_ERR_UNDEFINED_VAR  5
#define SEM_ERR_BAD_RETURN 6
#define SEM_ERR_INCOMPATIBLE_TYPES 7
#define SEM_ERR_OTHER 8
#define INTERNAL_ERR 99

///enumeration for tokens
typedef enum{
    //--------Special tokens------
    $WS = 0,       ///White space token
    $EOF,      /// End of file token EOF or ?>
    $START,    /// <?php
    //-------Keyword tokens--------
    $ELSE,
    $FLOAT,
    $FUNCTION,
    $IF,
    $INT,
    $NULL,
    $RETURN,
    $STRING,
    $VOID,
    $WHILE,
    //--------Variables and constants------
    $FID,       /// function names
    $VAR,       /// variable names staring with $
    $STR,       /// string constants in " "
    $NUMBER,    /// floats -> 12.564
    $INTEGER,   /// 123
    $EXPR,      /// everithing between = and ;
    //--------Operators----------
    $MUL,   /// *
    $DIV,   /// /
    $ADD,   /// +
    $SUB,   /// -
    $DOT,   /// .
    $OEQ,   /// =
    //-------Logical operators------
    $LT,    /// <
    $GT,    /// >
    $LTE,   /// <=
    $GTE,   /// >=
    $EQ,    /// ===
    $NEQ,   /// !==
    //-------Symbols-----------
    $COLON,     /// :
    $COMMA,     /// ,
    $SEMICOL,   /// ;
    $LPAR,      /// (
    $RPAR,      /// )
    $LCURL,     /// {
    $RCURL,     /// }
    $DOLLAR,     /// $
    $LEX_ERR,
    $QMARK,     /// ?
    $SHIFT,     /// < used in precedence syntactic analysis
    $REDUCE,     /// >
    $E
}Tok_type;


/// Enumeration for instructions
typedef enum {
    IMOVE = 0,
    ICREATEFRAME,
    IPUSHFRAME,
    IPOPFRAME,
    IDEFVAR,
    ICALL,
    IRETURN,
    IPUSHS,
    IPOPS,
    ICLEARS,
    IADD,
    ISUB,
    IMUL,
    IDIV,
    IIDIV,
    IADDS,
    ISUBS,
    IMULS,
    IDIVS,
    IIDIVS,
    ILT,
    IGT,
    IEQ,
    ILTS,
    IGTS,
    IEQS,
    IAND,
    IOR,
    INOT,
    IANDS,
    IORS,
    INOTS,
    IINT2FLOAT,
    IFLOAT2INT,
    IINT2CHAR,
    ISTRI2INT,
    IINT2FLOATS,
    IFLOAT2INTS,
    IINT2CHARS,
    ISTRI2INTS,
    IREAD,
    IWRITE,
    ICONCAT,
    ISTRLEN,
    IGETCHAR,
    ISETCHAR,
    ITYPE,
    ILABEL,
    IJUMP,
    IJUMPIFEQ,
    IJUMPIFNEQ,
    IJUMPIFEQS,
    IJUMPIFNEQS,
    IEXIT,
    IBREAK,
    IDPRINT,
    ICOMMENT
}I_type;




/// Location in input file (Size 16B)
typedef struct{
    char *file;
    int line;
    int character;
}Location;

/// Type of value
typedef enum{
    Value_string = 0,
    Value_int,
    Value_float,
    Value_void,
    Value_any,
    Value_function
}Value_type;

/// Type of instruction argument
typedef enum{
    Arg_var = 0,
    Arg_symb_int,
    Arg_symb_string,
    Arg_symb_float,
    Arg_type,
    Arg_label
}Iarg_type;

typedef enum{
    LF = 0,
    GF,
    TF
}Frames;

/// Function parameters when declaring a function to be stored in global symbol table
typedef struct{
    char** param_names;
    Value_type* param_types;
    int param_count;
    Value_type return_type;
}Func_data;

/// Union for storing values (Size 8B)
typedef union {long long int i; double f; char* str; Func_data *func;}Value;

/// Token structure (Size 32B)
typedef struct{
    Location loc;
    Tok_type type;
    Value value;
}Token;

/// Structure for saving symbol table data (for now without functions) (Size 32B)
typedef struct{
    Location defined;
    Value val;
    Value_type type;
    bool used;
}Stable_data;

/// Node for the symbol table binary tree (Size 56B)
typedef struct node{
    Stable_data data;
    char* name;
    struct node *left;
    struct node *right;
}Stable_node;

/// Pointer to next instruction when using different lengths of instruction
typedef union{struct list* l;struct list1* l1;struct list2* l2;struct list3* l3;}Arg_list_type;

/// Argument types (size 12B)
typedef struct{
    Iarg_type arg1;
    Iarg_type arg2;
    Iarg_type arg3;
}Iarg_types;

/// main instruction list for generating instructions (size 40B)
typedef struct list3{
    I_type type;
    unsigned encoded_arg_types;
    size_t next; ///Relative memory pointer -> addr_of_next = addr_of_current + next;
    Value arg1;
    Value arg2;
    Value arg3;
}IList_node3;

/// if there is time for optimization instruction list for generating instructions (size 32B)
typedef struct list2{
    I_type type;
    unsigned encoded_arg_types;
    size_t next; ///Relative memory pointer -> addr_of_next = addr_of_current + next;
    Value arg1;
    Value arg2;
}IList_node2;

/// if there is time for optimization instruction list for generating instructions (size 24B)
typedef struct list1{
    I_type type;
    unsigned encoded_arg_types;
    size_t next; ///Relative memory pointer -> addr_of_next = addr_of_current + next;
    Value arg1;
}IList_node1;

/// if there is time for optimization instruction list for generating instructions (size 16B)
typedef struct list{
    I_type type;
    unsigned encoded_arg_types;
    size_t next; ///Relative memory pointer -> addr_of_next = addr_of_current + next;
}IList_node;

typedef struct stack{
    void **Stack;
    unsigned max;
    unsigned top;
}Stack;

/// Keep program variables in one place (size 168B)
typedef struct{
    Stable_node *global_sym;
    Stable_node *local_sym;
    FILE *input;
    Token *token;
    Frames active_frame;
    Iarg_types Iargument_types;
    Arg_list_type Ilist;
    Stack Farg_stack;
}Program;

extern const char* Tok_names[];
extern const char* Instruction_names[][2];
extern const char* Value_names[];

void Stack_init(Stack *S);
void Stack_free(Stack *S);
void Stack_push(Stack *S, void *item);
void* Stack_top(Stack *S);
void* Stack_pop(Stack *S);

void Dprintf_token(Token tok);
void Dprintf_location(Location loc);
void Dprintf_Stable_data(Stable_data data);
void printf_location(Location loc);
unsigned encode_arg_types(Iarg_types arg_types);
void decode_arg_types(unsigned encoded,Iarg_types *arg_types);



#endif //IFJ22_GLOBAL_DEF_H
