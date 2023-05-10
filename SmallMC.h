// Implementace překladače imperativního jazka
// xletak00 Lukáš Leták

#ifndef SMALLMC_H
#define SMALLMC_H

//size defined to 170 to have all MC_items in one memory page -> 4096 / sizeof(MC_item)
#define FRAME_SIZE 170
#define INITIAL_FILENAME_COUNT 10

#include <stdlib.h>
#include <stdint.h>
typedef struct{
    void *MEM;
    size_t size;
    uint32_t file;
    uint32_t line;
}MC_item;

typedef struct{
    MC_item (*Frame)[FRAME_SIZE];
    unsigned Used_frames;
    unsigned Alocated_frames;
    unsigned last;
    char **File_names;
    unsigned Filenames_allocated;
}MC_table;

int MCTinit(unsigned Size);
void FreeAll(int debug);
void *MCmalloc(size_t Size, char *file, int line);
void *MCrealloc(void *ptr,size_t Size, char *file, int line);
void MCfree(void *ptr, char *file, int line);

#ifdef MC_ON

#define malloc(X) MCmalloc(X, __FILE__, __LINE__)
#define realloc(X,Y) MCrealloc(X, Y, __FILE__, __LINE__)
#define free(X) MCfree(X, __FILE__, __LINE__)

#else

#define MCTinit(X) ;
#define FreeAll(X) ;

#endif


#endif //SMALLMC_H
