CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -pedantic

all:
	$(CC) $(CFLAGS) comp_main.c lex_analyz.c lex_analyz.h syn_analyz.c syn_analyz.h expr.c expr.h sem_analyz.c sem_analyz.h code_gen.c code_gen.h sym_table.c sym_table.h inbuild.c inbuild.h global_def.h global_def.c SmallMC.c SmallMC.h I_list.h I_list.c -o IFJ2022