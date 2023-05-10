//
// Created by Lukáš on 30.09.2022.
//

#include <stdlib.h>
#include <string.h>
#include "inbuild.h"
#include "global_def.h"
#include "SmallMC.h"
#include "sym_table.h"
#include "code_gen.h"

/**
 * Function for creating function data for insertion to symtable
 * @param paramCount count of function parameters
 * @param ret_type function return type
 * @param param_names parameter names
 * @param param_types parameter types
 * @return pointer to alocated and initialized function_data
 */
Func_data *CreateFunction(int paramCount, Value_type ret_type,char** param_names, const Value_type *param_types){
   //Alocate function data
    Func_data *func = malloc(sizeof(Func_data));
    //TODO:Error handling
    //Copy initial data
    func->param_count = paramCount;
    func->return_type = ret_type;

    if(param_names == NULL)
    {
        func->param_names = NULL;
    }else
    {
        //Setup space and copy parameter names
        func->param_names = malloc(paramCount);
        //TODO:Error handling
        for(int i = 0; i<paramCount;i++)
        {
            func->param_names[i] = malloc(strlen(param_names[i]) + 1);
            //TODO:Error handling
            memcpy(func->param_names[i],param_names[i],strlen(param_names[i]) + 1);
        }
    }

    if(param_types == NULL)
    {
        func->param_types = NULL;
    }else
    {
        //setup space and copy parameter types
        func->param_types = malloc(paramCount);
        //TODO:Error handling
        for(int i = 0; i<paramCount;i++)
        {
            func->param_types[i] = param_types[i];
        }
    }
    return func;
}

void set_inbuild_type(Stable_data *d){
    d->type = Value_function;
    d->defined.file = "BUILD_IN";
    d->defined.line = 1;
    d->defined.character = 1;
}

/**
 * Function for setting up build-in functions. It inserts all build-in functions to symtable to have them known
 * @param prog Program structure
 * @return
 */
int SetupBuildins(Program *prog) {
    Stable_data *func_data = StableInsert(&prog->global_sym,"reads");
    set_inbuild_type(func_data);
    func_data->val.func = CreateFunction(0,Value_string,NULL,NULL);

    func_data = StableInsert(&prog->global_sym,"readi");
    set_inbuild_type(func_data);
    func_data->val.func = CreateFunction(0,Value_int,NULL,NULL);

    func_data = StableInsert(&prog->global_sym,"readf");
    set_inbuild_type(func_data);
    func_data->val.func = CreateFunction(0,Value_float,NULL,NULL);

    //Function write has negative param count because it has variable amount of parameters
    func_data = StableInsert(&prog->global_sym,"write");
    set_inbuild_type(func_data);
    func_data->val.func = CreateFunction(-1,Value_void,NULL,NULL);


    func_data = StableInsert(&prog->global_sym,"floatval");
    set_inbuild_type(func_data);
    Value_type *inputs = malloc(sizeof(Value_type)*5);
    //Value any, because converter functions convert from any type, and null param names, because they
    // do not need a name
    inputs[0] = Value_any;
    func_data->val.func = CreateFunction(1,Value_float,NULL,inputs);

    func_data = StableInsert(&prog->global_sym,"intval");
    set_inbuild_type(func_data);
    func_data->val.func = CreateFunction(1,Value_int,NULL,inputs);

    func_data = StableInsert(&prog->global_sym,"strval");
    set_inbuild_type(func_data);
    func_data->val.func = CreateFunction(1,Value_string,NULL,inputs);

    func_data = StableInsert(&prog->global_sym,"strlen");
    set_inbuild_type(func_data);
    char *strlennames[] = {
            "$s"
    };
    inputs[0] = Value_string;
    func_data->val.func = CreateFunction(1,Value_int,strlennames,inputs);

    func_data = StableInsert(&prog->global_sym,"substring");
    set_inbuild_type(func_data);
    char *substrnames[] = {
            "$s",
            "$i",
            "$j"
    };
    inputs[0] = Value_string;
    inputs[1] = Value_int;
    inputs[2] = Value_int;
    func_data->val.func = CreateFunction(3,Value_string,substrnames,inputs);

    func_data = StableInsert(&prog->global_sym,"ord");
    set_inbuild_type(func_data);
    char *ordnames[] = {
            "$c"
    };
    inputs[0] = Value_string;
    func_data->val.func = CreateFunction(1,Value_int,ordnames,inputs);

    func_data = StableInsert(&prog->global_sym,"chr");
    set_inbuild_type(func_data);
    char *chrnames[] = {
            "$i"
    };
    inputs[0] = Value_int;
    func_data->val.func = CreateFunction(1,Value_string,chrnames,inputs);

    free(inputs);
    return 0;

}




int GenerateBuildins(Program *prog) {
    GenerateDirectly(prog, "\n# Build-in function reads"
                           "\nLABEL reads"
                           "\nREAD GF@$return string"
                           "\nRETURN");

    GenerateDirectly(prog, "\n# Build-in function readi"
                           "\nLABEL readi"
                           "\nREAD GF@$return int"
                           "\nRETURN");

    GenerateDirectly(prog, "\n# Build-in function readf"
                           "\nLABEL readf"
                           "\nREAD GF@$return float"
                           "\nRETURN");

    GenerateDirectly(prog, "\n# Build-in function write"
                           "\nLABEL write"
                           "\nCREATEFRAME"
                           "\nPUSHFRAME"
                           "\nDEFVAR LF@cnt"
                           "\nDEFVAR LF@tmp"
                           "\nMOVE LF@cnt GF@$Farg_1"
                           "\nLABEL $write$loop_start"
                           "\nJUMPIFEQ $write$loop_end LF@cnt int@0"
                           "\nPOPS LF@tmp"
                           "\nWRITE LF@tmp"
                           "\nSUB LF@cnt LF@cnt int@1"
                           "\nJUMP $write$loop_start"
                           "\nLABEL $write$loop_end"
                           "\nPOPFRAME"
                           "\nRETURN");

    GenerateDirectly(prog, "\n# Build-in function floatval"
                           "\nLABEL floatval"
                           "\nCREATEFRAME"
                           "\nPUSHFRAME"
                           "\nDEFVAR LF@type"
                           "\nDEFVAR LF@var"
                           "\nPOPS LF@var"
                           "\nTYPE LF@type LF@var"
                           "\nJUMPIFEQ $floatval$int LF@type string@int"
                           "\nJUMPIFEQ $floatval$end LF@type string@float"
                           "\nJUMPIFEQ $floatval$nil LF@type string@nil"
                           "\nEXIT int@30"
                           "\nLABEL $floatval$nil"
                           "\nMOVE LF@var float@0x0p+0"
                           "\nJUMP $floatval$end"
                           "\nLABEL $floatval$int"
                           "\nINT2FLOAT LF@nil LF@var"
                           "\nLABEL $floatval$end"
                           "\nMOVE GF@$return LF@var"
                           "\nPOPFRAME"
                           "\nRETURN");

    GenerateDirectly(prog, "\n# Build-in function intval"
                           "\nLABEL intval"
                           "\nDEFVAR LF@type"
                           "\nDEFVAR LF@var"
                           "\nPOPS LF@var"
                           "\nTYPE LF@type LF@var"
                           "\nJUMPIFEQ $intval$float LF@type string@float"
                           "\nJUMPIFEQ $intval$end LF@type string@int"
                           "\nJUMPIFEQ $intval$nil LF@type string@nil"
                           "\nEXIT int@30"
                           "\nLABEL $intval$nil"
                           "\nMOVE LF@var float@0x0p+0"
                           "\nJUMP $intval$end"
                           "\nLABEL $intval$float"
                           "\nFLOAT2INT LF@var LF@var"
                           "\nLABEL $intval$end"
                           "\nMOVE GF@$return LF@var"
                           "\nPOPFRAME"
                           "\nRETURN");

    GenerateDirectly(prog, "\n# Build-in function strval"
                           "\nLABEL strval"
                           "\nDEFVAR LF@type"
                           "\nDEFVAR LF@var"
                           "\nPOPS LF@var"
                           "\nTYPE LF@type LF@var"
                           "\nJUMPIFEQ $strval$end LF@type string@string"
                           "\nJUMPIFEQ $strval$nil LF@type string@nil"
                           "\nEXIT int@30"
                           "\nLABEL $strval$nil"
                           "\nMOVE LF@var string@"
                           "\nLABEL $strval$end"
                           "\nMOVE GF@$return LF@var"
                           "\nPOPFRAME"
                           "\nRETURN");

    GenerateDirectly(prog, "\n# Build-in function strlen"
                           "\nLABEL strlen"
                           "\nCREATEFRAME"
                           "\nDEFVAR TF@tmp"
                           "\nPOPS TF@tmp"
                           "\nSTRLEN GF@$return TF@tmp"
                           "\nRETURN");

    GenerateDirectly(prog,"\n# Build-in function substring"
                          "\nLABEL substring"
                          "\nCREATEFRAME"
                          "\nPUSHFRAME"
                          "\nDEFVAR LF@s"
                          "\nPOPS LF@s"
                          "\nDEFVAR LF@i"
                          "\nPOPS LF@i"
                          "\nDEFVAR LF@j"
                          "\nPOPS LF@j"
                          "\nDEFVAR LF@slen"
                          "\nSTRLEN LF@slen LF@s"
                          "\nMOVE GF@$return nil@nil"
                          "\nJUMPIFEQ $substring$loop_end LF@slen int@0"
                          "\nDEFVAR LF@test"
                          "\nLT LF@test LF@i LF@slen"
                          "\nJUMPIFEQ $substring$loop_end LF@test bool@false"
                          "\nLT LF@test LF@i LF@j"
                          "\nJUMPIFEQ $substring$loop_end LF@test bool@false"
                          "\nMOVE GF@$return string@"
                          "\nDEFVAR LF@tmp"
                          "\nLABEL $substring$loop_start"
                          "\nJUMPIFEQ $substring$loop_end LF@i LF@j"
                          "\nJUMPIFEQ $substring$loop_end LF@i LF@slen"
                          "\nGETCHAR LF@tmp LF@s LF@i"
                          "\nCONCAT GF@$return GF@$return LF@tmp"
                          "\nADD LF@i LF@i int@1"
                          "\nJUMP $substring$loop_start"
                          "\nLABEL $substring$loop_end"
                          "\nPOPFRAME"
                          "\nRETURN");

    return GENERATED_OK;
}


