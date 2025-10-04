#include "enemy.h"
#include "config.h"
#include <math.h>
#include <string.h>

static int aabb_intersect(float x, float y, int w, int h, Platform *p) {
    return (x + w > p->x && x < p->x + p->w && y + h > p->y && y < p->y + p->h);
}

// --- Bullets (global simple pool) ---
#define MAX_ENEMY_BULLETS 128
static EnemyBullet g_bullets[MAX_ENEMY_BULLETS];
static int g_bullet_count = 0;

static void bullets_spawn(float x, float y, float vx) {
    if (g_bullet_count >= MAX_ENEMY_BULLETS) return;
    EnemyBullet *b = &g_bullets[g_bullet_count++];
    b->x = x; b->y = y;
    b->vx = vx;
    b->w = 8; b->h = 4;
    b->alive = 1;
}

void enemy_bullets_update(float dt) {
    // move and cull offscreen or dead
    int w = SCREEN_WIDTH * 2; // generous bounds
    for (int i = 0; i < g_bullet_count; i++) {
        EnemyBullet *b = &g_bullets[i];
        if (!b->alive) continue;
        b->x += b->vx * dt;
        if (b->x < -w || b->x > w * 2) b->alive = 0;
    }
}

const EnemyBullet* enemy_bullets_get(int *outCount) {
    if (outCount) *outCount = g_bullet_count;
    return g_bullets;
}

void enemy_bullets_reset() {
    memset(g_bullets, 0, sizeof(g_bullets));
    g_bullet_count = 0;
}

Enemy enemy_create_type(float x, float y, EnemyType type) {
    Enemy e;
    e.x = x; e.y = y;
    e.vx = 2.0f; e.vy = 0.0f;
    e.w = 40; e.h = 40;
    e.alive = 1;
    e.type = type;
    e.aiTimer = 0.0f;
    e.baseY = y;
    e.amplitude = 24.0f;
    e.omega = 0.1f; // умеренная синусоида (рад/кадр при dt~1)
    e.rangeL = x - 80.0f; e.rangeR = x + 80.0f;
    if (type == ENEMY_SHOOTER) {
        e.vx = 0.0f;
        e.w = 36; e.h = 36;
        e.aiTimer = 0.0f; // fire timer
    }
    if (type == ENEMY_SPIKE) {
        e.w = 36; e.h = 24; // ниже
    }
    return e;
}

Enemy enemy_create(float x, float y) {
    return enemy_create_type(x, y, ENEMY_PATROL);
}

void enemies_update_all(Enemy *enemies, int enemyCount, Platform *platforms, int platformCount, float dt) {
    for (int i = 0; i < enemyCount; i++) {
        Enemy *e = &enemies[i];
        if (!e->alive) continue;

        if (e->type == ENEMY_FLYING) {
            // Горизонтальный патруль в воздухе, без гравитации, по синусоиде по Y
            e->x += e->vx * dt;
            if (e->x < e->rangeL) { e->x = e->rangeL; e->vx = -e->vx; }
            if (e->x + e->w > e->rangeR) { e->x = e->rangeR - e->w; e->vx = -e->vx; }
            e->aiTimer += dt; // dt ~ 1 кадр
            e->y = e->baseY + e->amplitude * sinf(e->omega * e->aiTimer);
        } else if (e->type == ENEMY_SHOOTER) {
            // Стоит на месте (можно слегка покачивать взглядом), стреляет периодически
            // Псевдо-анимация: меняем знак взгляда каждые полпериода
            float period = 90.0f; // ~1.5 сек
            e->aiTimer += dt;
            int face = ((int)(e->aiTimer / (period * 0.5f)) % 2) ? 1 : -1;
            if (e->aiTimer >= period) {
                e->aiTimer = 0.0f;
                float bvx = face * (PLAYER_SPEED * 1.2f);
                bullets_spawn(e->x + e->w/2, e->y + e->h/2, bvx);
            }
            // Притяжение к земле, как у патруля
            e->y += e->vy * dt;
            e->vy += GRAVITY * dt;
            for (int j = 0; j < platformCount; j++) {
                Platform *p = &platforms[j]; if (p->h <= 0) continue;
                if (e->x + e->w > p->x && e->x < p->x + p->w) {
                    if (e->vy > 0 && e->y + e->h > p->y && (e->y + e->h - e->vy * dt) <= p->y) {
                        e->y = p->y - e->h; e->vy = 0;
                        break;
                    }
                }
            }
        } else {
            // Патруль/Прыгун: с гравитацией
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
            Platform *groundP = NULL;
            for (int j = 0; j < platformCount; j++) {
                Platform *p = &platforms[j];
                // пропускаем разрушившиеся платформы (h<=0)
                if (p->h <= 0) continue;
                // приземление сверху
                if (e->x + e->w > p->x && e->x < p->x + p->w) {
                    if (e->vy > 0 && e->y + e->h > p->y && (e->y + e->h - e->vy * dt) <= p->y) {
                        e->y = p->y - e->h;
                        e->vy = 0;
                        onGround = 1;
                        groundP = p;
                    }
                }
            }

            if (e->type == ENEMY_JUMPER) {
                // периодические прыжки, если стоим на земле
                e->aiTimer += dt;
                float period = 120.0f; // ~2 сек при 60 FPS
                // вычислим поддержку под ногами/впереди как ниже, чтобы не прыгать в пропасть
                int supported_current = 0;
                int supported_ahead = 0;
                const float eps = 1.0f;
                float foot_x = (e->vx >= 0) ? (e->x + e->w + 6.0f) : (e->x - 6.0f);
                float foot_y_level = e->y + e->h;
                for (int j = 0; j < platformCount; j++) {
                    Platform *p = &platforms[j];
                    if (p->h <= 0) continue;
                    if (e->x + e->w > p->x && e->x < p->x + p->w) {
                        if (fabsf(foot_y_level - p->y) <= eps) { supported_current = 1; }
                    }
                    if (foot_x >= p->x && foot_x <= p->x + p->w) {
                        if (fabsf(p->y - foot_y_level) <= 10.0f) { supported_ahead = 1; }
                    }
                    if (supported_current && supported_ahead) break;
                }
                int safe_margin_ok = 1;
                if (groundP) {
                    float margin = 12.0f;
                    if (e->x < groundP->x + margin || (e->x + e->w) > (groundP->x + groundP->w - margin)) {
                        safe_margin_ok = 0;
                    }
                }
                if (onGround && e->aiTimer >= period) {
                    if (supported_current && supported_ahead && safe_margin_ok) {
                        e->vy = JUMP_FORCE * 0.9f;
                        e->aiTimer = 0.0f;
                    } else {
                        // у края — развернуться и подождать перед следующей попыткой
                        e->vx = -e->vx;
                        e->aiTimer = period * 0.5f; // отложить прыжок
                    }
                }
            }

            // если на земле — проверим край платформы (разворачиваться у края до шага в пустоту)
            if (onGround) {
                int supported_current = 0;
                int supported_ahead = 0;
                const float eps = 1.0f;
                float foot_x = (e->vx >= 0) ? (e->x + e->w + 6.0f) : (e->x - 6.0f);
                float foot_y_level = e->y + e->h;
                for (int j = 0; j < platformCount; j++) {
                    Platform *p = &platforms[j];
                    if (p->h <= 0) continue;
                    if (e->x + e->w > p->x && e->x < p->x + p->w) {
                        if (fabsf(foot_y_level - p->y) <= eps) { supported_current = 1; }
                    }
                    if (foot_x >= p->x && foot_x <= p->x + p->w) {
                        if (fabsf(p->y - foot_y_level) <= 10.0f) { supported_ahead = 1; }
                    }
                    if (supported_current && supported_ahead) break;
                }
                if (!supported_ahead) e->vx = -e->vx;
            }
        }
    }
}
