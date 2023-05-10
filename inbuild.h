//
// Created by Lukáš on 30.09.2022.
//
#include "global_def.h"

#ifndef IFJ22_INBUILD_H
#define IFJ22_INBUILD_H

#define NO_BUILDINS 11

/**
 * Setups build in functions
 * @param prog program parameters
 * @return BUILDIN_OK or INTERNAL_ERR
 */
int SetupBuildins(Program *prog);
int GenerateBuildins(Program *prog);

#endif //IFJ22_INBUILD_H
