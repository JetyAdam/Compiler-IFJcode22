//
// Created by Lukáš on 16.10.2022.
//

#include "I_list.h"
#include "SmallMC.h"
#include "global_def.h"
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 64

void Ilist_insert(Arg_list_type *Ilist, Arg_list_type *new_node) {
    char *ptr;
    static int size = INIT_SIZE;
    static unsigned long free_space = 0;
    IList_node *new_node_conv = (IList_node *)new_node;
    //new_node_conv->next.l=NULL;

    unsigned long node_size = 16 + strtoul(Instruction_names[new_node_conv->type][1],&ptr,10) * 8;

    //printf("Size=%d, filled=%zu, \n",size,free_space);
    if(Ilist->l == NULL)
    {
        size = INIT_SIZE;
        free_space = 0;
        Ilist->l = malloc(size);
        memcpy(Ilist->l,(void*)new_node, node_size);
        Ilist->l->next=node_size;
    } else
    {

        if(size <= free_space + node_size)
        {
            size*=2;
            Ilist->l = realloc(Ilist->l,size);
            memset((void*)Ilist->l+size/2,0,size/2);
        }
        new_node_conv->next=node_size;
        memcpy((void*)Ilist->l+free_space,(void*)new_node, node_size);
    }
    free_space += node_size;
    //free(new_node);
}