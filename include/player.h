#ifndef PLAYER_H
#define PLAYER_H

typedef struct {
    float x, y;
    float vx, vy;
    int onGround;
} Player;

Player player_create(float x, float y);
void player_update(Player *p);
void player_jump(Player *p);
void player_move(Player *p, float dir);

#endif