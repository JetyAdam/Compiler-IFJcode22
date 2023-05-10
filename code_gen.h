//
// Created by Lukáš on 30.09.2022.
//
#include "global_def.h"

#ifndef IFJ22_CODE_GEN_H
#define IFJ22_CODE_GEN_H

/**
 * Generates code after analysis
 * @param prog Program parameters
 * @return GENERATED_OK or INTERNAL_ERR
 */
int GenerateOutput(Program *prog);
int GenerateFileHeader(Program *prog);
int GeneratePush(Program *prog);
int GenerateExprResult(Program *prog,char *name, Frames frame);
int GenerateDirectly(Program *prog, char *comment);
int GenerateVar(Program *prog, char *name,Frames frame);
int GenerateOperand(Program *prog, Tok_type type);
int GenerateLabel(Program *prog, char *Label_name);
int GenerateFuncArgument(Program *prog);
int GenerateFuncCall(Program *prog, int params, char* Func_name);
int GenerateWhileStart(Program *prog);
int GenerateWhileTest(Program *prog);
int GenerateWhileEnd(Program *prog);
int GenerateIfStart(Program *prog);
int GenerateIfTest(Program *prog);
int GenerateIfElse(Program *prog);
int GenerateIfEnd(Program *prog);
int GenerateFdefStart(Program *prog,char* Fname);
int GenerateFdefEnd(Program *prog);

#endif //IFJ22_CODE_GEN_H
