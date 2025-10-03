#include "enemy.h"
#include "config.h"

static int aabb_intersect(float x, float y, int w, int h, Platform *p) {
    return (x + w > p->x && x < p->x + p->w && y + h > p->y && y < p->y + p->h);
}

Enemy enemy_create(float x, float y) {
    Enemy e;
    e.x = x; e.y = y;
    e.vx = 2.0f; e.vy = 0.0f;
    e.w = 40; e.h = 40;
    e.alive = 1;
    return e;
}

void enemies_update_all(Enemy *enemies, int enemyCount, Platform *platforms, int platformCount, float dt) {
    for (int i = 0; i < enemyCount; i++) {
        Enemy *e = &enemies[i];
        if (!e->alive) continue;

        // горизонт
        e->x += e->vx * dt;
        // если уткнулся боком в платформу — развернуться
        for (int j = 0; j < platformCount; j++) {
            Platform *p = &platforms[j];
            if (aabb_intersect(e->x, e->y, e->w, e->h, p)) {
                if (e->vx > 0) e->x = p->x - e->w; else e->x = p->x + p->w;
                e->vx = -e->vx;
            }
        }

        // вертикаль
        e->y += e->vy * dt;
        e->vy += GRAVITY * dt;
        int onGround = 0;
        for (int j = 0; j < platformCount; j++) {
            Platform *p = &platforms[j];
            // приземление сверху
            if (e->x + e->w > p->x && e->x < p->x + p->w) {
                if (e->vy > 0 && e->y + e->h > p->y && (e->y + e->h - e->vy * dt) <= p->y) {
                    e->y = p->y - e->h;
                    e->vy = 0;
                    onGround = 1;
                }
            }
        }

        // если на земле — проверим край платформы (разворачиваться у края до шага в пустоту)
        if (onGround) {
            int supported_current = 0;
            int supported_ahead = 0;
            const float eps = 1.0f;
            // «датчик» у носка в направлении движения (на 6px вперёд)
            float foot_x = (e->vx >= 0) ? (e->x + e->w + 6.0f) : (e->x - 6.0f);
            float foot_y_level = e->y + e->h; // уровень подошвы
            for (int j = 0; j < platformCount; j++) {
                Platform *p = &platforms[j];
                // Поддержка под текущей позицией
                if (e->x + e->w > p->x && e->x < p->x + p->w) {
                    if (fabsf(foot_y_level - p->y) <= eps) { supported_current = 1; }
                }
                // Поддержка впереди
                if (foot_x >= p->x && foot_x <= p->x + p->w) {
                    if (fabsf(p->y - foot_y_level) <= 10.0f) { supported_ahead = 1; }
                }
                if (supported_current && supported_ahead) break;
            }
            // Если впереди нет поддержки — развернуться, не уходя в пустоту
            if (!supported_ahead) e->vx = -e->vx;
        }
    }
}
