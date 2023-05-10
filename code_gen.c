//
// Created by Lukáš on 30.09.2022.
//

#include "code_gen.h"
#include "global_def.h"
#include "SmallMC.h"
#include "inbuild.h"
#include "I_list.h"
#include <string.h>

#define ID(name) \
Stack name ## Stack;\
size_t name ## ID = 0; \
size_t GetNew ## name ## ID()\
{\
return name ## ID++;\
}

ID(While)
ID(If)
ID(Func)

/**
 * Function for optimizing push and immediate pop to move instruction
 * @param prog program data structure
 * @param optimal pointer to instruction list where the optimal code will be stored
 * @return
 */
int OptimizePush_Pop(Program *prog, Arg_list_type *optimal)
{
    Arg_list_type instruction = prog->Ilist;
    char *ptr;

    //Travere the unoptimal instruction list
    while(instruction.l->next!=0)
    {
        switch(instruction.l->type)
        {
            case IPUSHS:
                //if found pushs and the next instruction is pops generate move instruction
                if(((struct list*)((void*)instruction.l+instruction.l->next))->next!=0)
                {
                    if(((struct list*)((void*)instruction.l+instruction.l->next))->type == IPOPS)
                    {
                        IList_node2 new;
                        new.type=IMOVE;
                        new.next = 0;
                        //transfer arg types to new istruction
                        Iarg_types i1;
                        decode_arg_types(instruction.l->encoded_arg_types,&i1);
                        Iarg_types i2;
                        decode_arg_types(((struct list*)((void*)instruction.l+instruction.l->next))->encoded_arg_types,&i2);
                        prog->Iargument_types.arg1 = i2.arg1;
                        prog->Iargument_types.arg2 = i1.arg1;
                        new.encoded_arg_types = encode_arg_types(prog->Iargument_types);
                        new.arg1 = ((struct list1*)((void*)instruction.l+instruction.l->next))->arg1;
                        new.arg2 = instruction.l1->arg1;

                        Ilist_insert(optimal,(Arg_list_type *)&new);
                        //free(new);
                        instruction.l = ((void*)instruction.l+instruction.l->next);
                        break;
                    }
                }
            default:
                Ilist_insert(optimal,(Arg_list_type *)instruction.l);
                break;
        }
        instruction.l = ((void*)instruction.l+instruction.l->next);
    }
    return GENERATED_OK;
}

/**
 * Optimize the instruction list before generating output
 * @param prog program data structure
 * @return
 */
int OptimizeCode(Program *prog)
{
    Arg_list_type optimal;
    optimal.l = NULL;

    OptimizePush_Pop(prog,&optimal);
    free(prog->Ilist.l);
    prog->Ilist.l = optimal.l;

    return GENERATED_OK;
}

/**
 * Generate output from instruction list
 * @param prog program data structure
 * @return
 */
int GenerateOutput(Program *prog) {

    //Try to optimize before output
    OptimizeCode(prog);
    Stack_free(&WhileStack);
    Stack_free(&IfStack);

    Arg_list_type instruction = prog->Ilist;
    //printf("=========Generator output===============\n");
    char *ptr;
    //Traverse the whole list
    while(instruction.l->next!=0)
    {
        //switch based on instruction argument count
        switch(strtoul(Instruction_names[instruction.l->type][1],&ptr,10))
        {
            case 0:
                printf("%s\n",Instruction_names[instruction.l->type][0]);
                break;
            case 1:
                if(instruction.l1->type == ICOMMENT)
                {
                    printf("%s\n",instruction.l1->arg1.str);
                    free(instruction.l1->arg1.str);
                    break;
                }
                //Print the instruction
                printf("%s ",Instruction_names[instruction.l1->type][0]);
                //decode the type and based on that print prefix same with other arguments
                decode_arg_types(instruction.l1->encoded_arg_types,&prog->Iargument_types);
                switch(prog->Iargument_types.arg1)
                {
                    case Arg_symb_int:
                        printf("int@");
                        printf("%lld",instruction.l1->arg1.i);
                        break;
                    case Arg_symb_float:
                        printf("float@");
                        printf("%a",instruction.l1->arg1.f);
                        break;
                    case Arg_symb_string:
                        printf("string@");
                    default:
                        printf("%s",instruction.l1->arg1.str);
                        free(instruction.l1->arg1.str);
                        break;
                }
                printf("\n");
                break;
            case 2:
                printf("%s ",Instruction_names[instruction.l2->type][0]);
                decode_arg_types(instruction.l2->encoded_arg_types,&prog->Iargument_types);
                switch(prog->Iargument_types.arg1)
                {
                    case Arg_symb_int:
                        printf("int@");
                        printf("%lld ",instruction.l2->arg1.i);
                        break;
                    case Arg_symb_float:
                        printf("float@");
                        printf("%a ",instruction.l2->arg1.f);
                        break;
                    case Arg_symb_string:
                        printf("string@");
                    default:
                        printf("%s ",instruction.l2->arg1.str);
                        free(instruction.l2->arg1.str);
                        break;
                }
                switch(prog->Iargument_types.arg2)
                {
                    case Arg_symb_int:
                        printf("int@");
                        printf("%lld",instruction.l2->arg2.i);
                        break;
                    case Arg_symb_float:
                        printf("float@");
                        printf("%a",instruction.l2->arg2.f);
                        break;
                    case Arg_symb_string:
                        printf("string@");
                    default:
                        printf("%s",instruction.l2->arg2.str);
                        free(instruction.l2->arg2.str);
                        break;
                }
                printf("\n");
                break;
            case 3:
                printf("%s ",Instruction_names[instruction.l3->type][0]);
                decode_arg_types(instruction.l3->encoded_arg_types,&prog->Iargument_types);
                switch(prog->Iargument_types.arg1)
                {
                    case Arg_symb_int:
                        printf("int@");
                        printf("%lld",instruction.l3->arg1.i);
                        break;
                    case Arg_symb_float:
                        printf("float@");
                        printf("%a",instruction.l3->arg1.f);
                        break;
                    case Arg_symb_string:
                        printf("string@");
                    default:
                        printf("%s",instruction.l3->arg1.str);
                        free(instruction.l3->arg1.str);
                        break;
                }
                switch(prog->Iargument_types.arg2)
                {
                    case Arg_symb_int:
                        printf("int@");
                        printf("%lld",instruction.l3->arg2.i);
                        break;
                    case Arg_symb_float:
                        printf("float@");
                        printf("%a",instruction.l3->arg2.f);
                        break;
                    case Arg_symb_string:
                        printf("string@");
                    default:
                        printf("%s",instruction.l3->arg2.str);
                        free(instruction.l3->arg2.str);
                        break;
                }
                switch(prog->Iargument_types.arg3)
                {
                    case Arg_symb_int:
                        printf("int@");
                        printf("%lld",instruction.l3->arg3.i);
                        break;
                    case Arg_symb_float:
                        printf("float@");
                        printf("%a",instruction.l3->arg3.f);
                        break;
                    case Arg_symb_string:
                        printf("string@");
                    default:
                        printf("%s",instruction.l3->arg3.str);
                        free(instruction.l3->arg3.str);
                        break;
                }
                printf("\n");
                break;
        }
        instruction.l = ((void*)instruction.l+instruction.l->next);
    }
    //printf("=========Generator output===============\n");
    free(prog->Ilist.l);
    return 0;
}

/**
 * Generate file header witch all build-in functions and start of main program
 * @param prog program data structure
 * @return
 */
int GenerateFileHeader(Program *prog)
{
    GenerateDirectly(prog, ".IFJcode22");
    GenerateVar(prog,"$return",GF);
    GenerateVar(prog,"$Farg_1",GF);
    GenerateVar(prog,"$Farg_2",GF);
    GenerateVar(prog,"$Farg_3",GF);

    //Init while and if stacks
    Stack_init(&WhileStack);
    Stack_init(&IfStack);

    //create frame
    IList_node new;
    new.type = ICREATEFRAME;
    new.next = 0;
    Ilist_insert(&prog->Ilist, (Arg_list_type*)&new);
    GenerateDirectly(prog,"JUMP $main");

    SetupBuildins(prog);
    GenerateBuildins(prog);


    GenerateDirectly(prog,"\n#Start of main program"
                          "\nLABEL $main");
    return GENERATED_OK;
}

/**
 * Generate push instruction, when analyzing equations push all operands and than perform stack OP
 * @param prog program data structure
 * @return
 */
int GeneratePush(Program *prog)
{
    IList_node1 new;
    new.type=IPUSHS;
    new.next = 0;
    switch(prog->token->type)
    {
        case $VAR:
            new.arg1.str = malloc(3+strlen(prog->token->value.str)+1);
            memset(new.arg1.str,0,3+strlen(prog->token->value.str)+1);
            switch(prog->active_frame)
            {
                case LF:
                    strcat(new.arg1.str,"LF@");
                    break;
                case TF:
                    strcat(new.arg1.str,"TF@");
                    break;
                case GF:
                    strcat(new.arg1.str,"GF@");
                    break;
                default:
                    return INTERNAL_ERR;
            }
            strcat(new.arg1.str,prog->token->value.str);
            prog->Iargument_types.arg1 = Arg_var;
            break;
        case $STR:
            new.arg1.str = malloc(strlen(prog->token->value.str)+1);
            strcpy(new.arg1.str,prog->token->value.str);
            prog->Iargument_types.arg1 = Arg_symb_string;
            break;
        case $INTEGER:
            new.arg1.i = prog->token->value.i;
            prog->Iargument_types.arg1 = Arg_symb_int;
            break;
        case $NUMBER:
            new.arg1.f = prog->token->value.f;
            prog->Iargument_types.arg1 = Arg_symb_float;
            break;
        default:
            return INTERNAL_ERR;
    }
    new.encoded_arg_types = encode_arg_types(prog->Iargument_types);
    Ilist_insert(&prog->Ilist,  (Arg_list_type *)&new);
    return GENERATED_OK;
}


/**
 * Generate result of equation.
 * @param prog program data structure
 * @param name name of variable to store result
 * @param frame in witch frame is the variable
 * @return
 */
int GenerateExprResult(Program *prog,char *name, Frames frame)
{
    IList_node1 new;
    new.type=IPOPS;
    new.next = 0;
    new.arg1.str = malloc(3+strlen(name)+1);
    switch(frame)
    {
        case LF:
            strcpy(new.arg1.str,"LF@");
            break;
        case TF:
            strcpy(new.arg1.str,"TF@");
            break;
        case GF:
            strcpy(new.arg1.str,"GF@");
            break;
        default:
            return INTERNAL_ERR;
    }
    strcat(new.arg1.str,name);
    prog->Iargument_types.arg1 = Arg_var;
    new.encoded_arg_types = encode_arg_types(prog->Iargument_types);
    Ilist_insert(&prog->Ilist, (Arg_list_type*)&new);
    return GENERATED_OK;
}


/**
 * Generate stack operation based on operand
 * @param prog program data structure
 * @return
 */
int GenerateOperand(Program *prog,Tok_type type)
{
    IList_node new;
    switch(type)
    {
        case $MUL:
            new.type = IMULS;
            break;
        case $DIV:
            new.type = IDIVS;
            break;
        case $ADD:
            new.type = IADDS;
            break;
        case $SUB:
            new.type = ISUBS;
            break;
        case $DOT:
            UNIMPLEMENTED("String dot operation");
            break;
        case $LT:
            new.type = ILTS;
            break;
        case $GT:
            new.type = IGTS;
            break;
        case $LTE:
            UNIMPLEMENTED("Less than or eq operation");
            break;
        case $GTE:
            UNIMPLEMENTED("Greater than or eq operation");
            break;
        case $EQ:
            new.type = IEQS;
            break;
        case $NEQ:
            UNIMPLEMENTED("Not eq operation");
            break;
        default:
            return INTERNAL_ERR;
            break;
    }
    new.next = 0;
    Ilist_insert(&prog->Ilist, (Arg_list_type*)&new);
    return GENERATED_OK;
}

/**
 * Generate directly to output file (use # for generating comments)
 * @param prog program data structure
 * @param comment comment content
 * @return
 */
int GenerateDirectly(Program *prog, char *comment)
{
    IList_node1 new;
    new.type=ICOMMENT;
    new.next = 0;

    new.arg1.str = malloc(strlen(comment)+1);
    strcpy(new.arg1.str, comment);

    prog->Iargument_types.arg1 = Arg_symb_string;
    new.encoded_arg_types = encode_arg_types(prog->Iargument_types);
    Ilist_insert(&prog->Ilist, (Arg_list_type*)&new);
    return GENERATED_OK;
}


/**
 * Generate variable definition
 * @param prog program data structure
 * @param name Name of var
 * @param frame frame in witch to generate
 * @return
 */
int GenerateVar(Program *prog, char *name,Frames frame)
{
    IList_node1 new;
    new.type=IDEFVAR;
    new.next = 0;
    new.arg1.str = malloc(3+strlen(name)+1);
    switch(frame)
    {
        case LF:
            strcpy(new.arg1.str,"LF@");
            break;
        case TF:
            strcpy(new.arg1.str,"TF@");
            break;
        case GF:
            strcpy(new.arg1.str,"GF@");
            break;
        default:
            return INTERNAL_ERR;
    }
    strcat(new.arg1.str,name);
    prog->Iargument_types.arg1 = Arg_var;
    new.encoded_arg_types = encode_arg_types(prog->Iargument_types);
    Ilist_insert( &prog->Ilist, (Arg_list_type *)&new);
    return GENERATED_OK;
}

/**
 * Generates label for jumps
 * @param prog program data structure
 * @param Label_name name of label
 * @return
 */
int GenerateLabel(Program *prog, char *Label_name) {
    IList_node1 new;
    new.type=ILABEL;
    new.next = 0;
    new.arg1.str = malloc(strlen(Label_name)+1);
    strcpy(new.arg1.str,Label_name);
    prog->Iargument_types.arg1 = Arg_label;
    new.encoded_arg_types = encode_arg_types(prog->Iargument_types);
    Ilist_insert( &prog->Ilist, (Arg_list_type *)&new);
    return GENERATED_OK;
}

int GenerateFuncArgument(Program *prog){
    Token *t = malloc(sizeof(*t));
    memcpy(t,prog->token,sizeof(*t));
    switch(prog->token->type)
    {
        case $VAR:
        case $STR:
            t->value.str = malloc(strlen(prog->token->value.str)+1);
            memcpy(t->value.str,prog->token->value.str,strlen(prog->token->value.str)+1);
            break;
        default:
            break;
    }
    Stack_push(&prog->Farg_stack,t);
    return GENERATED_OK;
}

int GenerateFuncCall(Program *prog, int params, char* Func_name){
    Token *t;
    int p = params;
    GenerateDirectly(prog,"#Function call start");
    while(p>0)
    {
        t = Stack_pop(&prog->Farg_stack);
        memcpy(prog->token,t,sizeof(*t));

        GeneratePush(prog);
        switch(prog->token->type)
        {
            case $VAR:
            case $STR:
                free(t->value.str);
                break;
            default:
                break;
        }
        free(t);
        p--;
    }
    t = malloc(sizeof(*t));
    t->type = $INTEGER;
    t->value.i = params;
    memcpy(prog->token,t,sizeof(*t));
    GeneratePush(prog);
    GenerateExprResult(prog,"$Farg_1", GF);
    free(t);
    IList_node1 new;
    new.type=ICALL;
    new.next = 0;
    new.arg1.str = malloc(strlen(Func_name)+1);
    strcpy(new.arg1.str,Func_name);
    prog->Iargument_types.arg1 = Arg_label;
    new.encoded_arg_types = encode_arg_types(prog->Iargument_types);
    Ilist_insert( &prog->Ilist, (Arg_list_type *)&new);
    return GENERATED_OK;
}

int GenerateWhileStart(Program *prog){
    GetNewWhileID();
    Stack_push(&WhileStack,(void *)WhileID);
    char buffer[40]={0};
    sprintf(buffer,"%%while_%zu_start",WhileID);
    GenerateLabel(prog,buffer);
    return GENERATED_OK;
}

int GenerateWhileTest(Program *prog){
    char buffer[100]={0};
    sprintf(buffer,"JUMPIFEQ %%while_%zu_end GF@$return int@0", (size_t)Stack_top(&WhileStack));
    GenerateDirectly(prog,buffer);
    return GENERATED_OK;
}

int GenerateWhileEnd(Program *prog){
    char buffer[100]={0};
    sprintf(buffer,"JUMP %%while_%zu_start", (size_t)Stack_top(&WhileStack));
    GenerateDirectly(prog,buffer);
    sprintf(buffer,"%%while_%zu_end", (size_t)Stack_top(&WhileStack));
    GenerateLabel(prog,buffer);
    Stack_pop(&WhileStack);
    return GENERATED_OK;
}

int GenerateIfStart(Program *prog){
    GetNewIfID();
    Stack_push(&IfStack,(void *)IfID);
    char buffer[40]={0};
    sprintf(buffer,"#%%if_%zu_start",IfID);
    GenerateDirectly(prog,buffer);
    return GENERATED_OK;
}

int GenerateIfTest(Program *prog){
    char buffer[100]={0};
    sprintf(buffer,"JUMPIFEQ %%if_%zu_else GF@$return int@0", (size_t)Stack_top(&IfStack));
    GenerateDirectly(prog,buffer);
    return GENERATED_OK;
}

int GenerateIfElse(Program *prog){
    char buffer[100]={0};
    sprintf(buffer,"JUMP %%if_%zu_end", (size_t)Stack_top(&IfStack));
    GenerateDirectly(prog,buffer);
    sprintf(buffer,"%%if_%zu_else", (size_t)Stack_top(&IfStack));
    GenerateLabel(prog,buffer);
    return GENERATED_OK;
}

int GenerateIfEnd(Program *prog){
    char buffer[100]={0};
    sprintf(buffer,"%%if_%zu_end", (size_t)Stack_top(&IfStack));
    GenerateLabel(prog,buffer);
    Stack_pop(&IfStack);
    return GENERATED_OK;
}

int GenerateFdefStart(Program *prog,char* Fname){
    GetNewFuncID();
    char buffer[40]={0};
    sprintf(buffer,"JUMP %%func_%zu_end",FuncID);
    GenerateDirectly(prog,buffer);
    char *s = malloc(sizeof(strlen(Fname) + 2));
    sprintf(s,"%s",Fname);
    GenerateLabel(prog,s);
    free(s);
    return GENERATED_OK;
}

int GenerateFdefEnd(Program *prog){
    char buffer[40]={0};
    GenerateDirectly(prog,"RETURN");
    sprintf(buffer,"%%func_%zu_end",FuncID);
    GenerateLabel(prog,buffer);
    return GENERATED_OK;
}
