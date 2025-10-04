#ifndef ENEMY_H
#define ENEMY_H

#include "platform.h"

typedef enum {
    ENEMY_PATROL = 0,
    ENEMY_FLYING = 1,
    ENEMY_JUMPER = 2,
    ENEMY_SHOOTER = 3,
    ENEMY_SPIKE = 4
} EnemyType;

typedef struct {
    float x, y;
    float vx, vy;
    int w, h;
    int alive;
    EnemyType type;
    // Generic AI helpers
    float aiTimer;     // frames counter or phase
    float baseY;       // for flying sine
    float amplitude;   // for flying sine
    float omega;       // for flying sine
    float rangeL, rangeR; // patrol range for flying/patrol
} Enemy;

Enemy enemy_create(float x, float y);
Enemy enemy_create_type(float x, float y, EnemyType type);
void enemies_update_all(Enemy *enemies, int enemyCount, Platform *platforms, int platformCount, float dt);

// Bullets fired by shooters
typedef struct {
    float x, y;
    float vx;
    int w, h;
    int alive;
} EnemyBullet;

void enemy_bullets_update(float dt);
const EnemyBullet* enemy_bullets_get(int *outCount);
void enemy_bullets_reset();

#endif
