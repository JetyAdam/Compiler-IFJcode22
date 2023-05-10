cmake_minimum_required(VERSION 3.16)
project(SmallGC C)

set(CMAKE_C_STANDARD 99)

add_executable(SmallGC main.c SmallMC.c SmallMC.h global_def.h test.c test.h)

add_executable(Compiler comp_main.c lex_analyz.c lex_analyz.h syn_analyz.c syn_analyz.h expr.c expr.h
        sem_analyz.c sem_analyz.h code_gen.c code_gen.h sym_table.c sym_table.h inbuild.c inbuild.h global_def.h
        global_def.c SmallMC.c SmallMC.h I_list.h I_list.c)