#include "player.h"
#include "config.h"
#include <math.h>

Player player_create(float x, float y) {
    Player p = {x, y, y, 0, 0, 0, 3, 0, 0,
                1, /* facing */
                1, /* frameIdx 1..8 */
                0.0f /* animTimer */};
    return p;
}

void player_update(Player *p, Platform *platforms, int platformCount, float dt) {
    // Запомним предыдущую позицию по Y для детекции "стомпа" по врагам
    p->lastY = p->y;
    // Горизонтальное движение и коллизии
    p->x += p->vx * dt;
    for (int i = 0; i < platformCount; i++) {
        Platform* plat = &platforms[i];
        if (p->x + PLAYER_SIZE > plat->x && p->x < plat->x + plat->w &&
            p->y + PLAYER_SIZE > plat->y && p->y < plat->y + plat->h) {
            if (p->vx > 0) { // Движение вправо
                p->x = plat->x - PLAYER_SIZE;
            } else if (p->vx < 0) { // Движение влево
                p->x = plat->x + plat->w;
            }
        }
    }

    // Вертикальное движение и коллизии
    p->y += p->vy * dt;
    p->vy += GRAVITY * dt;
    p->onGround = 0;

    for (int i = 0; i < platformCount; i++) {
        Platform* plat = &platforms[i];
        if (p->x + PLAYER_SIZE > plat->x && p->x < plat->x + plat->w) {
            // Коллизия сверху платформы (приземление)
            if (p->vy > 0 && p->y + PLAYER_SIZE > plat->y && (p->y + PLAYER_SIZE - p->vy * dt) <= plat->y) {
                p->y = plat->y - PLAYER_SIZE;
                p->vy = 0;
                p->onGround = 1;
                // Если платформа движется, «везём» игрока
                if (plat->type == PLATFORM_MOVING) {
                    p->x += plat->vx * dt;
                }
            }
            // Коллизия снизу платформы (удар головой)
            if (p->vy < 0 && p->y < plat->y + plat->h && (p->y - p->vy * dt) >= (plat->y + plat->h)) {
                p->y = plat->y + plat->h;
                p->vy = 0;
            }
        }
    }

    // Подстраховка: если стоим на платформе почти точно (погрешность по float), считаем что на земле
    if (!p->onGround && p->vy >= 0) {
        for (int i = 0; i < platformCount; i++) {
            Platform* plat = &platforms[i];
            if (p->x + PLAYER_SIZE > plat->x && p->x < plat->x + plat->w) {
                float dy = plat->y - (p->y + PLAYER_SIZE);
                if (fabsf(dy) <= 1.0f) {
                    p->y = plat->y - PLAYER_SIZE;
                    p->vy = 0;
                    p->onGround = 1;
                    if (plat->type == PLATFORM_MOVING) {
                        p->x += plat->vx * dt;
                    }
                    break;
                }
            }
        }
    }

    // Убрали бесконечную землю: падение обрабатывается в игровом цикле (респаун)

    // Анимация: 12 FPS для бега, кадры 1..8. В простое держим 1-й кадр
    // dt здесь нормализован ~1.0 при 60 FPS, значит шаг кадра ~5.0
    const float RUN_FRAME_STEP = 3.0f; // 60/12
    if (fabsf(p->vx) > 0.01f && p->onGround) {
        p->animTimer += dt;
        while (p->animTimer >= RUN_FRAME_STEP) {
            p->animTimer -= RUN_FRAME_STEP;
            p->frameIdx += 1;
            if (p->frameIdx > 8) p->frameIdx = 1;
        }
    } else {
        p->frameIdx = 1;
        // не сбрасываем animTimer полностью, чтобы переход выглядел плавнее
        if (p->animTimer > RUN_FRAME_STEP) p->animTimer = fmodf(p->animTimer, RUN_FRAME_STEP);
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
    if (dir > 0.01f) p->facing = 1;
    else if (dir < -0.01f) p->facing = -1;
}