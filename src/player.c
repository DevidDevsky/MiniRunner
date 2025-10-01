#include "player.h"
#include "config.h"

Player player_create(float x, float y) {
    Player p = {x, y, 0, 0, 0};
    return p;
}

void player_update(Player *p) {
    p->x += p->vx;
    p->y += p->vy;
    p->vy += GRAVITY;

    if (p->y >= GROUND_Y - PLAYER_SIZE) {
        p->y = GROUND_Y - PLAYER_SIZE;
        p->vy = 0;
        p->onGround = 1;
    }
}

void player_jump(Player *p) {
    if (p->onGround) {
        p->vy = JUMP_FORCE;
        p->onGround = 0;
    }
}

void player_move(Player *p, float dir) {
    p->vx = dir * PLAYER_SPEED;
}