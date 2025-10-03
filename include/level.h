#ifndef LEVEL_H
#define LEVEL_H

#include "platform.h"

typedef struct {
    Platform *platforms;
    int platformCount;
    int startPlatform;
    int finishPlatform;
} Level;

Level level_create(int difficulty);
void level_destroy(Level *lvl);

#endif