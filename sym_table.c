//
// Created by Lukáš on 30.09.2022.
//

#include "sym_table.h"
#include "global_def.h"
#include "SmallMC.h"
#include <string.h>

void StableInit(Stable_node *root) {
    if(root != NULL){
        root = root->left = root->right = NULL;
    }
}

Stable_data *StableInsert(Stable_node **root, char *name){
    if((*root) == NULL){
        (*root) = malloc(sizeof(*(*root)));
        if((*root) == NULL){
            return (void*)INTERNAL_ERR;
        }
        (*root)->name = name;
        (*root)->left = (*root)->right = NULL;
        (*root)->data.type = Value_any;
        return &((*root)->data);
    } else if(strcmp((*root)->name, name) > 0){
       return StableInsert(&(*root)->right, name);
    } else if(strcmp((*root)->name, name) < 0){
        return StableInsert(&(*root)->left, name);
    } else //Duplicate
    {
        //Also returns data, duplicate check id done with data.type != Value_any.
        return &((*root)->data);
    }
}

Stable_data *StableSearch(Stable_node *root, char *name){
    if(root == NULL){
        return NULL;
    } else if(strcmp(root->name, name) == 0){
        return &root->data;
    } else if(strcmp(root->name, name) > 0){
        return StableSearch(root->right, name);
    } else { // strcmp(root->name, name) < 0
        return StableSearch(root->left, name);
    } 
}

void StableFree(Stable_node *root) {
    if(root != NULL){
        if(root->data.type == Value_function)
        {
            if(root->data.val.func->param_count>0)
            {
                if(root->data.val.func->param_names != NULL){
                    for(int i = 0; i<root->data.val.func->param_count; i++)
                    {
                        free(root->data.val.func->param_names[i]);
                    }
                    free(root->data.val.func->param_names);
                }
                free(root->data.val.func->param_types);
            }
            free(root->data.val.func);
        }
        StableFree(root->left);
        StableFree(root->right);
        free(root);
    }
}
