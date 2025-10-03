#include "player.h"
#include "config.h"

Player player_create(float x, float y) {
    Player p = {x, y, 0, 0, 0};
    return p;
}

void player_update(Player *p, Platform *platforms, int platformCount, float dt) {
    // движение
    p->x += p->vx * dt;
    p->y += p->vy * dt;
    p->vy += GRAVITY * dt;

    p->onGround = 0;

    // проверка коллизий со всеми платформами
    for (int i = 0; i < platformCount; i++) {
        Platform* plat = &platforms[i];

        float player_bottom = p->y + PLAYER_SIZE;
        float platform_top = plat->y;

        // Проверяем, что игрок пересекается с платформой по X
        if (p->x + PLAYER_SIZE > plat->x && p->x < plat->x + plat->w) {
            // Проверяем, что игрок приземляется на платформу сверху
            if (player_bottom >= platform_top && (player_bottom - p->vy * dt) <= platform_top) {
                p->y = platform_top - PLAYER_SIZE;
                p->vy = 0;
                p->onGround = 1;
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