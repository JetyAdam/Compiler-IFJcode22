//
// Created by Lukáš on 04.09.2022.
//
#include <stdlib.h>
#include "global_def.h"
#include "SmallMC.h"
void testm(int i)
{
    char *s = malloc(i);
    char *t = malloc(i);
    s = realloc(s,i*2);
    free(t);
}