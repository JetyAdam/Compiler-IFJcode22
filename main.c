#include <stdio.h>
#include <stdlib.h>
#include "global_def.h"
#include "SmallMC.h"
#include <time.h>
#include "test.h"
/*
int main(){
    MCTinit(2);

    char* str;
    str = malloc(12);

    str = realloc(str,35);

    str = malloc(15);


    testm(32);

    str = malloc(19);

    free(str);


    str = malloc(16);

    FreeAll(1);
}
*/
int main() {
    FILE *f;
    int c;
    f = fopen("/proc/self/status","r");
    while((c=fgetc(f))!=EOF)
    {
        putchar(c);
    }
    printf("=======================\n");
    fclose(f);

    clock_t start = clock();


    MCTinit(2);

    char *str;
    clock_t init = clock();
    for(int i = 0; i<10;i++)
    {
        testm(1);
        str = malloc(1);
    }
    clock_t Mtime = clock();


    f = fopen("/proc/self/status","r");
    while((c=fgetc(f))!=EOF)
    {
        putchar(c);
    }
    printf("=======================\n");
    fclose(f);
    //free(0x414141);

    FreeAll(1);
    clock_t end = clock();

    f = fopen("/proc/self/status","r");
    while((c=fgetc(f))!=EOF)
    {
        putchar(c);
    }
    fclose(f);
    printf("=======================\n");


    double time = (double)(end-start) / CLOCKS_PER_SEC;
    printf("EXEC TIME: %lf\n",time);
    time = (double)(init-start) / CLOCKS_PER_SEC;
    printf("init time: %lf\n",time);
    time = (double)(Mtime-init) / CLOCKS_PER_SEC;
    printf("Malloc time: %lf\n",time);
    time = (double)(end-Mtime) / CLOCKS_PER_SEC;
    printf("free time: %lf\n",time);



    return 0;
}
