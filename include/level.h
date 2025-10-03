#ifndef LEVEL_H
#define LEVEL_H

#include "platform.h"
#include "enemy.h"

typedef struct {
    Platform *platforms;
    int platformCount;
    int startPlatform;
    int finishPlatform;
    int difficulty; // current difficulty/level index
    // Enemies
    Enemy *enemies;
    int enemyCount;
} Level;

Level level_create(int difficulty);
void level_destroy(Level *lvl);

#endif