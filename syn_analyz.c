/*
    Created by Lukáš on 30.09.2022.

    Syntaktická analýza metodou rekurzivního sestupu v kombinaci
    s precedenční analýzou
*/
#include "syn_analyz.h"
#include "global_def.h"
#include "lex_analyz.h"
#include "sym_table.h"
#include "code_gen.h"
#include "SmallMC.h"
#include <string.h>

void get_next_token(Program *prog){
    if(GetNextToken(prog->token) == TOKEN_OK){
        if(prog->token->type == $WS){
            get_next_token(prog);
        }
        return;
    }

    exit(LEX_ERR);
}

// rule 53 <expr> -> $EXPR
static int expr(Program *prog){
    //TODO: implement
    //Call expression parser
    exit(SYNTAX_ERR);
}

// rule <term> -> $VAR | $STR | $NUMBER | $INTEGER | $NULL
static int term(Program *prog){
    GenerateFuncArgument(prog);

    if(prog->token->type == $VAR){
        //TODO: jestli je to promenna, tak zkontrolovat, jestli je definovana
        return SYNTAX_OK;
    } else if(prog->token->type == $STR){
        return SYNTAX_OK;
    } else if(prog->token->type == $NUMBER){
        return SYNTAX_OK;
    } else if(prog->token->type == $INTEGER){
        return SYNTAX_OK;
    } else if(prog->token->type == $NULL){
        return SYNTAX_OK;
    }

    exit(SYNTAX_ERR);
}

static int term_seq1(Program *prog, int *count){
    if(prog->token->type == $COMMA){
        (*count)++;
        get_next_token(prog);
        term(prog);
        get_next_token(prog);
        if(prog->token->type == $COMMA){
            term_seq1(prog, count);
            return SYNTAX_OK;
        } else if(prog->token->type == $RPAR){
            return SYNTAX_OK;
        }
        term_seq1(prog, count);
    } else if(prog->token->type == $RPAR){
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

static int term_seq(Program *prog, int *count){
    if(prog->token->type == $VAR || prog->token->type == $STR || prog->token->type == $NUMBER || prog->token->type == $INTEGER || prog->token->type == $NULL){
        (*count)++;
        term(prog);
        get_next_token(prog);

        if(prog->token->type == $COMMA){
            term_seq1(prog, count);
            return SYNTAX_OK;
        } else if(prog->token->type == $RPAR){
            return SYNTAX_OK;
        }
    } else if(prog->token->type == $RPAR){
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

//TODO: může být return_val pouze výraz? Nemám tu mít třeba i <return_val> -> $VAR?
//TODO: co je zde vše potřeba ošetřovat? Jak zjistím typ výrazu? Jak ošetřím typ funkce a typ returnu?
// rules <return_val> -> <expr> | EMPTY
static int return_val(Program *prog){
    if(prog->token->type == $EXPR){
        expr(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $SEMICOL){
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

// rule 36 <return_stmt> -> $RETURN <return_val>
static int return_stmt(Program *prog){
    if(prog->token->type == $RETURN){
        get_next_token(prog);
        return_val(prog);

        //TODO: Generate return value?
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

// rule 24 <func_call> -> $FID ( <term_seq> )
static int func_call(Program *prog){
    //TODO: jestli jsou parametry stejného typu
    //TODO: jestli jsou parametry definovány

    Stable_data *func = StableSearch(prog->global_sym, prog->token->value.str);
    char *name = malloc(strlen(prog->token->value.str)+1);
    memcpy(name, prog->token->value.str, strlen(prog->token->value.str)+1);
    //jestli je funkce definována
    if(func == NULL){
            exit(SEM_ERR_UNDEFINED_FUNC);
    }


    if(prog->token->type == $FID){
        get_next_token(prog);
        if(prog->token->type == $LPAR){
            get_next_token(prog);
            int count = 0;
            term_seq(prog, &count);
            get_next_token(prog);

           //TODO: jestli má funkce stejný počet parametrů
            //if(count == ){
            //}
            GenerateFuncCall(prog, count, name);
            free(name);
            return SYNTAX_OK;
        }
    }

    exit(SYNTAX_ERR);
}

// rules 49,50 <var_assign> -> <func_call> | <expr>
static int var_assign(Program *prog){
    if(prog->token->type == $FID){
        func_call(prog);

        //TODO: where and how to store var value?

        return SYNTAX_OK;
    } else if(prog->token->type == $EXPR){
        expr(prog);

        //TODO: where and how to store var value?

        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

// rule 48 <var_def_dec> -> $var = <var_assign>
static int var_def_dec(Program *prog){
    if(prog->token->type == $VAR){
        get_next_token(prog);
        if(prog->token->type == $OEQ){
            get_next_token(prog);
            var_assign(prog);

            return SYNTAX_OK;
        }
    }
    exit(SYNTAX_ERR);
}

// rules 21, 22 <stmt> -> <func_call> | <var_def_dec>
static int stmt(Program *prog){
    if(prog->token->type == $FID){
        func_call(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $VAR){
        var_def_dec(prog);
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

static int body1(Program *prog){
    if(prog->token->type == $FID){
        stmt(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $VAR){

        //add variable to local symbol table
        if(StableSearch(prog->local_sym, prog->token->value.str) == NULL){
            //add variable to symbol table
            Stable_data *var = StableInsert(&prog->local_sym, prog->token->value.str);
            //TODO generate??
            GenerateVar(prog, prog->token->value.str, LF);
            stmt(prog);
            return SYNTAX_OK;
        } else {
            //TODO: redefinition of variable
            stmt(prog);
            return SYNTAX_OK;
        }

    } else if(prog->token->type == $IF){
        stmt(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $WHILE){
        stmt(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $RETURN){
        stmt(prog);
        return SYNTAX_OK;
    }

    exit(SYNTAX_ERR);
}

static int body2(Program *prog){
    if(prog->token->type == $FID || prog->token->type == $VAR || prog->token->type == $IF || prog->token->type == $WHILE || prog->token->type == $RETURN){
        body1(prog);
        if(prog->token->type == $FID || prog->token->type == $VAR || prog->token->type == $IF || prog->token->type == $WHILE || prog->token->type == $RETURN){
            body2(prog);
            return SYNTAX_OK;
        } else if(prog->token->type == $RCURL){
            return SYNTAX_OK;
        }
    } else if(prog->token->type == $RCURL){
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

// rule <body> -> <body1> <body2>
static int body(Program *prog){
        //init local symbol table
        //StableInit(prog->local_sym);

        if(prog->token->type == $FID || prog->token->type == $VAR || prog->token->type == $IF || prog->token->type == $WHILE || prog->token->type == $RETURN){
        body1(prog);
        get_next_token(prog);
        body2(prog);

        //free local symbol table
        StableFree(prog->local_sym);
        return SYNTAX_OK;
    } else if(prog->token->type = $RCURL){
        //free local symbol table
        StableFree(prog->local_sym);
        return SYNTAX_OK;
    }

    exit(SYNTAX_ERR);
}

// rule 51 <if_stmt> -> $IF ( <expr> ) { <body> } $ELSE { <body> }
static int if_stmt(Program *prog){
    if(prog->token->type == $IF){

        GenerateIfStart(prog);

        get_next_token(prog);
        if(prog->token->type == $LPAR){
            get_next_token(prog);
            expr(prog);

            //right?!
            GenerateIfTest(prog);

            get_next_token(prog);
            if(prog->token->type == $RPAR){
                get_next_token(prog);
                if(prog->token->type == $LCURL){
                    get_next_token(prog);
                    body(prog);
                    get_next_token(prog);
                        if(prog->token->type == $ELSE){

                            GenerateIfElse(prog);

                            get_next_token(prog);
                            if(prog->token->type == $LCURL){
                                get_next_token(prog);
                                body(prog);

                                    GenerateIfEnd(prog);

                                    return SYNTAX_OK;
                            }
                        }
                }
            }
        }
    }
    exit(SYNTAX_ERR);
}

// rule 52 <while_stmt> -> $WHILE ( <expr> ) { <body> }
static int while_stmt(Program *prog){
    if(prog->token->type == $WHILE){

        GenerateWhileStart(prog);

        get_next_token(prog);
        if(prog->token->type == $LPAR){
            get_next_token(prog);
            expr(prog);

            //TODO: right?!
            GenerateWhileTest(prog);

            get_next_token(prog);
            if(prog->token->type == $RPAR){
                get_next_token(prog);
                if(prog->token->type == $LCURL){
                    get_next_token(prog);
                    body(prog);

                        GenerateWhileEnd(prog);

                        return SYNTAX_OK;
                    }
            }
        }
    }
    exit(SYNTAX_ERR);
}

// rule <type1> -> $VOID | $FLOAT | $STRING | $INT
static int type1(Program *prog){
    if(prog->token->type == $VOID){
        return SYNTAX_OK;
    } else if(prog->token->type == $FLOAT){
        return SYNTAX_OK;
    } else if(prog->token->type == $STRING){
        return SYNTAX_OK;
    } else if(prog->token->type == $INT){
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

// rule <type> -> ? <type1> | <type1>
static int type(Program *prog){
    if(prog->token->type == $QMARK){
        get_next_token(prog);
        type1(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $VOID || prog->token->type == $FLOAT || prog->token->type == $STRING || prog->token->type == $INT) {
        type1(prog);
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

// rule <param> -> <type> $var
static int param(Program *prog){
    if(prog->token->type == $VOID || prog->token->type == $FLOAT || prog->token->type == $STRING || prog->token->type == $INT){
        type(prog);

        //TODO: Add param type to symbol table

        get_next_token(prog);
        if(prog->token->type == $VAR){

            //TODO: save to function local symbol table

            //TODO: generateFuncArgument?
            return SYNTAX_OK;
        }
    }
    exit(SYNTAX_ERR);
}

// rules <param1> -> , <param> <param1> | <empty>
static int param1(Program *prog){
    if(prog->token->type == $COMMA){
        get_next_token(prog);
        param(prog);
        get_next_token(prog);
        if(prog->token->type = $COMMA){
            param1(prog);
            return SYNTAX_OK;
        } else if(prog->token->type == $RPAR){
            return SYNTAX_OK;
        }
        return SYNTAX_OK;
    } else if(prog->token->type == $RPAR){
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

// rules 25,26 <params> -> <param> <param1> | EMPTY
static int params(Program *prog){
    if(prog->token->type == $VOID || $FLOAT || $STRING || $INT){
        param(prog);
        get_next_token(prog);
        if(prog->token->type == $COMMA){
            param1(prog);
            return SYNTAX_OK;
        } else if(prog->token->type == $RPAR){
            return SYNTAX_OK;
        }
    } else if(prog->token->type == $RPAR){
        //žádné argumenty
        return SYNTAX_OK;
    }
    exit(SYNTAX_ERR);
}

//rule 23 <func_def_dec> -> $FUNCTION $FID ( <params> ) : <type> { <body> }
static int func_def_dec(Program *prog){
    if(prog->token->type == $FUNCTION){
        get_next_token(prog);
        if(prog->token->type == $FID){

            //if function is already in symbol table, then error 3
            if(StableSearch(prog->global_sym, prog->token->value.str) != NULL){
                exit(SEM_ERR_REDEFINED_FUNC);
            }

            //add function to symbol table
            Stable_data *func = StableInsert(prog->global_sym, prog->token->value.str);

            //generate function start
            GenerateFdefStart(prog, prog->token->value.str);

            get_next_token(prog);
            if(prog->token->type == $LPAR){
                get_next_token(prog);

                params(prog);

                    get_next_token(prog);
                    if(prog->token->type == $COLON){
                        get_next_token(prog);
                        type(prog);

                        //add function type
                        func->val.func->return_type = prog->token->type;

                        get_next_token(prog);
                        if(prog->token->type == $LCURL){
                            get_next_token(prog);
                            body(prog);

                                GenerateFdefEnd(prog);

                                return SYNTAX_OK;
                            }
                        }
            }
        }
    }

    exit(SYNTAX_ERR);
}

// rules 5-9 <code1> -> <func_def_dec> | <stmt> ; | <if_stmt> | <while_stmt> | <return_stmt> ;
static int code1(Program *prog){
    if(prog->token->type == $FUNCTION){
        func_def_dec(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $FID){
        //function call: function has to be in symbol table
        stmt(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $VAR){

        if(StableSearch(prog->global_sym, prog->token->value.str) == NULL){
            //add variable to symbol table
            Stable_data *var = StableInsert(&prog->global_sym, prog->token->value.str);
            //TODO generate??
            GenerateVar(prog, prog->token->value.str, GF);

            stmt(prog);
            return SYNTAX_OK;
        } else {
            //TODO: redefinition of variable
            stmt(prog);
            return SYNTAX_OK;
        }

    } else if(prog->token->type == $IF){
        if_stmt(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $WHILE){
        while_stmt(prog);
        return SYNTAX_OK;
    } else if(prog->token->type == $RETURN){
        return_stmt(prog);
        return SYNTAX_OK;
    }

    exit(SYNTAX_ERR);
}

// rules 10,11 <code2> -> <code1> <code2> | <endprolog>
static int code2(Program *prog){
    if(prog->token->type == $FUNCTION || prog->token->type == $FID || prog->token->type == $VAR || prog->token->type == $IF || prog->token->type == $WHILE || prog->token->type == $RETURN){
        code1(prog);
        get_next_token(prog);
        code2(prog);
        return SYNTAX_OK;
    } else if(prog->token->type = $EOF){
        return SYNTAX_OK;
    }
}

// rules 3,4 <code> -> <code1> <code2> | $EOF
static int code(Program *prog){

    //init global symbol table
    //StableInit(prog->global_sym);

    if(prog->token->type == $FUNCTION || prog->token->type == $FID || prog->token->type == $VAR || prog->token->type == $IF || prog->token->type == $WHILE || prog->token->type == $RETURN){
        code1(prog);
        get_next_token(prog);
        code2(prog);
        return SYNTAX_OK;
    } else if(prog->token->type = $EOF){
        return SYNTAX_OK;
    }

    //free global symbol table
    StableFree(prog->global_sym);

    exit(SYNTAX_ERR);
}

// rule 2 <prolog> -> $START declare ( strict_types = 1 ) ;
static int prolog(Program *prog){
    if(prog->token->type == $START){
        get_next_token(prog);
        if((prog->token->type == $FID) && (strcmp(prog->token->value.str, "declare") == 0)){
            get_next_token(prog);
            if(prog->token->type == $LPAR){
                get_next_token(prog);
                if((prog->token->type == $FID) && (strcmp(prog->token->value.str, "strict_types") == 0)){
                    get_next_token(prog);
                    if(prog->token->type == $OEQ){
                        get_next_token(prog);
                        if(prog->token->type == $INTEGER && prog->token->value.i == 1){
                            get_next_token(prog);
                            if(prog->token->type == $RPAR){
                                get_next_token(prog);
                                if(prog->token->type == $SEMICOL){

                                    //generate prolog code
                                    GenerateFileHeader(prog);

                                    get_next_token(prog);
                                    return SYNTAX_OK;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    exit(SYNTAX_ERR);
}

// rule 1 <program> -> <prolog> <code>
static int program(Program *prog){

    int err;

    err = prolog(prog);

    if(prog->token->type == $FUNCTION || prog->token->type == $FID || prog->token->type == $VAR || prog->token->type == $IF || prog->token->type == $WHILE || prog->token->type == $RETURN || prog->token->type == $EOF){
        err = code(prog);
    }

    if(err){
        exit(SYNTAX_ERR);
    }else{
        return err;
    }

}

// start of recursive descent
int start_analyzing(Program prog) {
    prog.token = malloc(sizeof(Token));

    Location loc = {.file=NULL,.line=0,.character=0};
    prog.token->type = $INTEGER;
    prog.token->value.i = 0;
    prog.token->loc = loc;

    prog.token->loc.file = "../test.txt";

            prog.active_frame = GF;
            prog.Ilist.l = NULL;
            prog.Iargument_types.arg1 = Arg_symb_int;
            prog.Iargument_types.arg2 = Arg_symb_int;
            prog.Iargument_types.arg3 = Arg_symb_int;

            prog.global_sym = NULL;
            prog.local_sym = NULL;
            prog.input = NULL;

    Stack_init(&prog.Farg_stack);

    SetInputFile(fopen(prog.token->loc.file, "r"));

    get_next_token(&prog);

    program(&prog);

    GenerateOutput(&prog);

    free(prog.token);
    Clear_char_stack();
    Stack_free(&prog.Farg_stack);

    StableFree(prog.global_sym);

    return SYNTAX_OK;
}