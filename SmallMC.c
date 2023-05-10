// Implementace překladače imperativního jazka
// xletak00 Lukáš Leták

#include "SmallMC.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#undef MCTinit
#undef FreeAll

MC_table *MCTable=NULL;

/**
 * Initializes structure for keeping track of <b>malloc()</b> and <b>free()</b>
 * @param Size Initial size of pre-allocated memory (255 * size).
 * @returns If allocation fails returns -1.
 */
int MCTinit(unsigned Size)
{
    if(MCTable!=NULL)
    {
        fprintf(stderr,"Double MC definition\n");
        return -1;
    }
    MCTable=malloc(sizeof(*MCTable));
    if(MCTable == NULL)
    {
        fprintf(stderr,"%s:%d: malloc(size = %zu) -> Malloc Error: %s\n", __FILE__, __LINE__ - 2, sizeof(*MCTable), strerror(errno));
        //exit(MALLOC_ERROR);
        return -1;
    }
    MCTable->Used_frames=0;
    MCTable->last=0;
    MCTable->File_names = malloc(INITIAL_FILENAME_COUNT*sizeof(*MCTable->File_names));
    MCTable->Filenames_allocated = INITIAL_FILENAME_COUNT;
    for(int i = 0; i<INITIAL_FILENAME_COUNT; i++)
    {
        MCTable->File_names[i]=NULL;
    }


    MCTable->Frame=malloc(Size * sizeof(MC_item [FRAME_SIZE]));
    if(MCTable->Frame==NULL) {
        fprintf(stderr,"%s:%d: malloc(size = %zu) -> Malloc Error: %s\n", __FILE__, __LINE__ - 2, Size * sizeof(MC_item [FRAME_SIZE]), strerror(errno));
        free(MCTable);
        //exit(MALLOC_ERROR);
        return -1;
    }

    MCTable->Alocated_frames=Size;
    for(int i=0;i<Size;i++)
    {
        for(int j=0;j<FRAME_SIZE;j++)
        {
            MCTable->Frame[i][j].MEM=NULL;
            MCTable->Frame[i][j].file=0;
            MCTable->Frame[i][j].line=0;
            MCTable->Frame[i][j].size=0;
        }
    }

    return 0;
}

/**
 * Frees residual memory and notifies user if there was memory allocated but not freed.
 *
 * <p>Free all pointers left in table -> there should not be any pointers left but if there are
 * notify user of possible memory leaks and free them and the whole structure<p>
 * @param debug If set print residual memory info
 */
void FreeAll(int debug)
{
    //
    for(int i=0;i<MCTable->Alocated_frames;i++)
    {
        for(int j=0;j<FRAME_SIZE;j++)
        {
            if(MCTable->Frame[i][j].MEM!=NULL)
            {
                if(debug > 0)
                    fprintf(stderr, "%s:%d: UNFREED MEMORY OF SIZE: %zu\n",MCTable->File_names[MCTable->Frame[i][j].file],MCTable->Frame[i][j].line,MCTable->Frame[i][j].size);
                free(MCTable->Frame[i][j].MEM);
            }
        }
    }
    free(MCTable->Frame);
    MCTable->Frame=NULL;

    for(int i=0; i<MCTable->Filenames_allocated;i++)
    {
        if(MCTable->File_names[i]!=NULL)
        {
            free(MCTable->File_names[i]);
        }
    }
    free(MCTable->File_names);

    free(MCTable);
    MCTable=NULL;
}

/**
 * Finds string in array or first free index.
 * @param Array NULL terminated array, where ihe string is supposed to be.
 * @param name string to find.
 * @param size size of the array.
 * @returns <b>Positive</b> (index + 1) to array if found or <b>negative</b> (index - 1) if not found and 0 if there are no free spaces left.
 */
int Contains_Filneme(char **Array, char *name,unsigned size)
{
    for(int i=0; i<size;i++)
    {
        if(Array[i] == NULL)
        {
            return -i-1;
        }
        if(strcmp(Array[i],name)==0)
        {
            return i+1;
        }
    }
    return 0;
}

/**
 * Adds new filename "name" to "Array" at "index".
 * @param Array array of names.
 * @param index index where to add.
 * @param name name that is added.
 */
void Add_new_fileneme(char **Array,unsigned index, char *name)
{
    Array[index] = malloc(strlen(name) + 1);
    memset(Array[index],0,strlen(name) + 1);
    strcpy(Array[index],name);
}

/**
 * Alocates memory like malloc but keeps copy of pointer and stores file and line data with it
 * @param Size Amount of bytes to be allocated.
 * @param file File in witch <b>malloc()</b> was called.
 * @param line Line at witch <b>malloc()</b> was called.
 * @returns Pointer to memory.
 */
void *MCmalloc(size_t Size, char *file, int line)
{
    //get the memory
    void *tmp=malloc(Size);
    if(tmp==NULL) {
        fprintf(stderr,"%s:%d: malloc(size = %zu) -> Malloc Error: %s\n",file, line, Size, strerror(errno));
        return tmp;
    }

    //find free spot for the memory to keep record of it
    size_t x=MCTable->last;
    while(x<FRAME_SIZE)
    {
        //if free spot is found, keep it in x
        if(MCTable->Frame[MCTable->Used_frames][x].MEM==NULL)
            break;
        x++;
    }
    if(x==FRAME_SIZE)
    {
        //if free spot is not found get next free frame
        x=0;
        if(MCTable->Used_frames<MCTable->Alocated_frames-1)
        {
            MCTable->Used_frames++;
        }
        else
        {
            //if all frame are full allocate more frames safely
            MCTable->Used_frames++;
            MCTable->Alocated_frames = MCTable->Alocated_frames * 2;
            MC_item (*MC_BIGER)[FRAME_SIZE]=malloc(MCTable->Alocated_frames * sizeof(MC_item [FRAME_SIZE]));
            if(MC_BIGER==NULL)
            {
                //if alocating more frames fails ?free all and exit?
                fprintf(stderr, "FATAL LIBRARY ERROR: ALLOCATING MORE FRAMES FAILED, EXITING\n");
                fprintf(stderr,"%s:%d: malloc(size = %zu) -> Malloc Error: %s\n", __FILE__, __LINE__ - 5, MCTable->Alocated_frames * sizeof(MC_item *[FRAME_SIZE]), strerror(errno));
                return NULL;

                //FreeAll(0);
                //exit(MALLOC_ERROR);

            }
            else
            {
                //memcopy
                for(int i=0;i<MCTable->Alocated_frames;i++)
                {
                    for(int j=0;j<FRAME_SIZE;j++)
                    {
                        if(i<MCTable->Alocated_frames/2)
                        {
                            MC_BIGER[i][j].MEM=MCTable->Frame[i][j].MEM;
                            MC_BIGER[i][j].file=MCTable->Frame[i][j].file;
                            MC_BIGER[i][j].line=MCTable->Frame[i][j].line;
                            MC_BIGER[i][j].size=MCTable->Frame[i][j].size;
                        }
                        else
                        {
                            MC_BIGER[i][j].MEM=NULL;
                            MC_BIGER[i][j].file=0;
                            MC_BIGER[i][j].line=0;
                            MC_BIGER[i][j].size=0;
                        }

                    }

                }
                free(MCTable->Frame);
                MCTable->Frame=MC_BIGER;
            }
        }
    }
    //make copy of the pointer to track it and add aditional information
    MCTable->Frame[MCTable->Used_frames][x].MEM=tmp;

    int index = Contains_Filneme(MCTable->File_names,file,MCTable->Filenames_allocated);
    if(index == 0){
        //make more space for file names and add the name
        MCTable->Filenames_allocated = MCTable->Filenames_allocated * 2;
        MCTable->File_names = realloc(MCTable->File_names,MCTable->Filenames_allocated*sizeof(*MCTable->File_names));
        if (MCTable->File_names == NULL) {
            fprintf(stderr, "%s:%d: realloc(size = %u) -> Realloc Error: %s\n", __FILE__, __LINE__ - 2, MCTable->Filenames_allocated, strerror(errno));
            return MCTable->File_names;
        }
        for(unsigned i = MCTable->Filenames_allocated/2; i<MCTable->Filenames_allocated; i++)
        {
            MCTable->File_names[i]=NULL;
        }
        Add_new_fileneme(MCTable->File_names,MCTable->Filenames_allocated/2,file);
        MCTable->Frame[MCTable->Used_frames][x].file=MCTable->Filenames_allocated/2;
    }
    else if(index>0)
    {
        //if name is found just take an index to it
        MCTable->Frame[MCTable->Used_frames][x].file=index-1;
    }
    else
    {
        //if name is not found add new name and take index to it
        index=-index;
        Add_new_fileneme(MCTable->File_names,index-1,file);
        MCTable->Frame[MCTable->Used_frames][x].file=index-1;
    }

    MCTable->Frame[MCTable->Used_frames][x].line = line;
    MCTable->Frame[MCTable->Used_frames][x].size = Size;

    MCTable->last=x;
    return tmp;
}

/**
 * Same as realloc, but updates the additional data.
 * @param ptr Pointer to memory to be reallocated.
 * @param Size Amount of bytes to be allocated.
 * @param file File in witch <b>malloc()</b> was called.
 * @param line Line at witch <b>malloc()</b> was called.
 * @returns Pointer to memory.
 */
void *MCrealloc(void *ptr,size_t Size, char *file, int line)
{
    //get last location in frame
    unsigned x=MCTable->Used_frames;
    unsigned y=MCTable->last;

    //if input pointer is null, print out error
    if(ptr==NULL)
    {
        fprintf(stderr,"%s:%d: REALOC on freed memory\n",file,line);
        return NULL;
    }

    //find the allocated pointer in table
    while(ptr!=MCTable->Frame[x][y].MEM)
    {
        //if not found print out error
        if(x==0 && y==0)
        {
            fprintf(stderr,"%s:%d: REALOC on NOT allocated memory\n",file,line);
            return NULL;
        }
        if(y==0)
        {
            x--;
            y=FRAME_SIZE;
        }
        y--;
    }

    //do the realloc
    void *tmp=realloc(MCTable->Frame[x][y].MEM,Size);
    if(tmp==NULL){
        fprintf(stderr,"%s:%d: realloc(size = %zu) -> Realloc Error: %s\n",file, line, Size, strerror(errno));
        return tmp;
    }

    //Update aditional information
    MCTable->Frame[x][y].MEM=tmp;
    int index = Contains_Filneme(MCTable->File_names,file,MCTable->Filenames_allocated);
    if(index == 0){
        //make more space for file names and add the name
        MCTable->Filenames_allocated = MCTable->Filenames_allocated * 2;
        MCTable->File_names = realloc(MCTable->File_names,MCTable->Filenames_allocated*sizeof(*MCTable->File_names));
        if (MCTable->File_names == NULL) {
            fprintf(stderr, "%s:%d: realloc(size = %u) -> Realloc Error: %s\n", __FILE__, __LINE__ - 2, MCTable->Filenames_allocated, strerror(errno));
            return MCTable->File_names;
        }
        for(unsigned i = MCTable->Filenames_allocated/2; i<MCTable->Filenames_allocated; i++)
        {
            MCTable->File_names[i]=NULL;
        }
        Add_new_fileneme(MCTable->File_names,MCTable->Filenames_allocated/2,file);
        MCTable->Frame[x][y].file=MCTable->Filenames_allocated/2;
    }
    else if(index>0)
    {
        //if name is found just take an index to it
        MCTable->Frame[x][y].file=index-1;
    }
    else
    {
        //if name is not found add new name and take index to it
        index=-index;
        Add_new_fileneme(MCTable->File_names,index-1,file);
        MCTable->Frame[x][y].file=index-1;
    }

    MCTable->Frame[x][y].line = line;
    MCTable->Frame[x][y].size = Size;

    return tmp;
}

/**
 * Frees residual memory and notifies user if there was memory allocated but not freed.
 * @param ptr Pointer to memory to be freed.
 * @param file File in witch <b>malloc()</b> was called.
 * @param line Line at witch <b>malloc()</b> was called.
 */
void MCfree(void *ptr, char *file, int line)
{
    //start at the latest pointer
    unsigned x=MCTable->Used_frames;
    unsigned y=MCTable->last;

    if(ptr==NULL)
    {
        fprintf(stderr,"%s:%d: FREE on freed memory\n",file,line);
        return;
    }

    //find the pointer -> linear search
    while(ptr!=MCTable->Frame[x][y].MEM)
    {
        if(x==0 && y==0)
        {
            fprintf(stderr,"%s:%d: FREE on NOT alocated memory\n",file,line);
            return;
        }
        if(y==0)
        {
            x--;
            y=FRAME_SIZE;
        }
        y--;
    }

    //free the pointer and its associated data
    free(ptr);
    if(MCTable->Used_frames==0 && MCTable->last==0)
    {
        MCTable->Frame[x][y].MEM=NULL;
        MCTable->Frame[x][y].file=0;
        MCTable->Frame[x][y].line=0;
        MCTable->Frame[x][y].size = 0;
        return;
    }else
    {
        //to keep the table without holes switch the freed with the last values
        MCTable->Frame[x][y].MEM=MCTable->Frame[MCTable->Used_frames][MCTable->last].MEM;
        MCTable->Frame[MCTable->Used_frames][MCTable->last].MEM=NULL;
        MCTable->Frame[x][y].file=MCTable->Frame[MCTable->Used_frames][MCTable->last].file;
        MCTable->Frame[MCTable->Used_frames][MCTable->last].file=0;
        MCTable->Frame[x][y].line=MCTable->Frame[MCTable->Used_frames][MCTable->last].line;
        MCTable->Frame[MCTable->Used_frames][MCTable->last].line=0;
        MCTable->Frame[x][y].size=MCTable->Frame[MCTable->Used_frames][MCTable->last].size;
        MCTable->Frame[MCTable->Used_frames][MCTable->last].size=0;
        if(MCTable->last == 0)
        {
            MCTable->Used_frames--;
            MCTable->last=FRAME_SIZE;
        }
        MCTable->last--;
        return;
    }
}

