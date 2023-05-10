//
// Created by Lukáš on 30.09.2022.
//
#include "global_def.h"

#ifndef IFJ22_SYM_TABLE_H
#define IFJ22_SYM_TABLE_H

/**
 * Initialization of symbol table structure
 * @param root root node.
 */
void StableInit(Stable_node *root);

/**
 * Insert into symbol table
 * @param root pointer to root node
 * @param name name(key) for adding
 * @return NULL if failed, pointer to data otherwise
 */
Stable_data* StableInsert(Stable_node **root, char* name);

/**
 * Find name in symbol table
 * @param root pointer to root node
 * @param name name(key) for adding
 * @return NULL if failed, pointer to data otherwise
 */
Stable_data* StableSearch(Stable_node *root, char* name);

/**
 * Free the whole symbol table
 * @param root pointer to root node
 */
void StableFree(Stable_node *root);

#endif //IFJ22_SYM_TABLE_H
