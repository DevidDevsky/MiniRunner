#ifndef ENEMY_H
#define ENEMY_H

#include "platform.h"

typedef struct {
    float x, y;
    float vx, vy;
    int w, h;
    int alive;
} Enemy;

Enemy enemy_create(float x, float y);
void enemies_update_all(Enemy *enemies, int enemyCount, Platform *platforms, int platformCount, float dt);

#endif
