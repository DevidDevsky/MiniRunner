#include "player.h"
#include "config.h"

Player player_create(float x, float y) {
    Player p = {x, y, 0, 0, 0};
    return p;
}

// проверка пересечения прямоугольников
static int rect_intersect(float x1, float y1, float w1, float h1,
                          float x2, float y2, float w2, float h2) {
    return !(x1 + w1 <= x2 || x1 >= x2 + w2 ||
             y1 + h1 <= y2 || y1 >= y2 + h2);
}

void player_update(Player *p, Platform *platforms, int platformCount, float dt) {
    // движение
    p->x += p->vx * dt;
    p->y += p->vy * dt;
    p->vy += GRAVITY * dt;

    p->onGround = 0;

    // проверка коллизий со всеми платформами
    for (int i = 0; i < platformCount; i++) {
        Platform *plat = &platforms[i];

        if (rect_intersect(p->x, p->y, PLAYER_SIZE, PLAYER_SIZE,
                           plat->x, plat->y, plat->w, plat->h)) {
            float pxCenter = p->x + PLAYER_SIZE / 2.0f;
            float pyCenter = p->y + PLAYER_SIZE / 2.0f;
            float platCenterX = plat->x + plat->w / 2.0f;
            float platCenterY = plat->y + plat->h / 2.0f;

            float dx = pxCenter - platCenterX;
            float dy = pyCenter - platCenterY;

            float overlapX = (PLAYER_SIZE / 2.0f + plat->w / 2.0f) - fabsf(dx);
            float overlapY = (PLAYER_SIZE / 2.0f + plat->h / 2.0f) - fabsf(dy);

            if (overlapX < overlapY) {
                // столкновение по X
                if (dx > 0) {
                    p->x += overlapX; // вправо
                } else {
                    p->x -= overlapX; // влево
                }
                p->vx = 0;
            } else {
                // столкновение по Y
                if (dy > 0) {
                    // удар снизу (головой)
                    p->y += overlapY;
                    p->vy = 0;
                } else {
                    // стоим сверху
                    p->y -= overlapY;
                    p->vy = 0;
                    p->onGround = 1;
                }
            }
        }
    }

    // ограничение снизу (земля)
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