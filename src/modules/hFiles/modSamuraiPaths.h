/*
    / / / / / / / / / / / / /
  / / modSamuraiPaths.h / /
/ / / / / / / / / / / / /

local executable path helpers

*/

#ifndef MODSAMURAIPATHS_H
#define MODSAMURAIPATHS_H

#include "modSamuraiCommon.h"

void GetExeDir (void);

void PathCombineLocal (
    char       *out,
    const char *rel
);

#endif
