#ifndef LEVEL_H
#define LEVEL_H

#include "platform.h"

typedef struct {
    Platform *platforms;
    int platformCount;
} Level;

Level level_create();
void level_destroy(Level *lvl);

#endif