#ifndef PLAYER_H
#define PLAYER_H

#include "platform.h"

typedef struct {
    float x, y;
    float lastY; // previous Y for stomp detection
    float vx, vy;
    int onGround;
    int lives; // player lives
    int invFrames; // frames of post-damage invulnerability
    int respawnLockFrames; // frames to lock horizontal movement after respawn
    int facing; // 1 = right, -1 = left
    int frameIdx; // current animation frame (1..8)
    float animTimer; // accumulates dt for frame stepping
} Player;

Player player_create(float x, float y);
void player_update(Player *p, Platform *platforms, int platformCount, float dt);
void player_jump(Player *p);
void player_move(Player *p, float dir);

#endif