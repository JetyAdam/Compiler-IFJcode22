//
// Created by Lukáš on 30.09.2022.
//

//#include <stdio.h>
//#include <stdlib.h>
#include "global_def.h"
#include "SmallMC.h"
//#include "string.h"
#include "sym_table.h"
#include "syn_analyz.h"
#include "lex_analyz.h"
#include "string.h"
#include "code_gen.h"
#include "expr.h"

int main(){
    MCTinit(2);
    
    Program PROG;
    
    start_analyzing(PROG);

    FreeAll(1);
}
