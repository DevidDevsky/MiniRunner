#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "level.h"
#include "config.h"
#include "difficulty.h"
#include "enemy.h"

#define MAX_PLATFORMS_COUNT 256

// Расчет максимальной высоты (по модулю) и дальности прыжка при текущей физике
static void calculate_jump_params(float *max_jump_up, float *max_jump_distance, float *air_time) {
    const float v0 = JUMP_FORCE;        // отрицательное
    const float g  = GRAVITY;           // положительное
    const float t_peak = -v0 / g;       // время до вершины
    const float h_peak = v0 * t_peak + 0.5f * g * t_peak * t_peak; // отрицательное значение
    *max_jump_up = fabsf(h_peak);       // высота прыжка вверх (положительная)
    *air_time = 2.0f * t_peak;          // полный полёт
    *max_jump_distance = PLAYER_SPEED * (*air_time);
}

// Проверка пересечения по X с запасом (margin)
static int intersects_x(float x, float w, float ox, float ow, float margin) {
    float a0 = x - margin;
    float a1 = x + w + margin;
    float b0 = ox;
    float b1 = ox + ow;
    return !(a1 <= b0 || a0 >= b1);
}

// Сдвигает x вправо, чтобы исключить любое перекрытие отрезков X с уже добавленными платформами
static float resolve_x_no_overlap(float desired_x, float w, Level *lvl, float margin) {
    float x = desired_x;
    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < lvl->platformCount; i++) {
            Platform *p = &lvl->platforms[i];
            if (intersects_x(x, w, p->x, p->w, margin)) {
                x = p->x + p->w + margin; // сдвигаем после конца существующей платформы
                changed = 1;
            }
        }
    }
    return x;
}

Level level_create(int difficulty) {
    Level lvl;
    lvl.platforms = malloc(sizeof(Platform) * MAX_PLATFORMS_COUNT);
    lvl.platformCount = 0;
    lvl.difficulty = difficulty;

    float max_jump_up, max_jump_dx, air_time;
    calculate_jump_params(&max_jump_up, &max_jump_dx, &air_time);

    DifficultyProfile prof; difficulty_compute_profile(difficulty, &prof);

    // Коэффициенты безопасности
    const float GAP_MAX = fminf(max_jump_dx * prof.gap_max_k, max_jump_dx * 0.95f);
    const float GAP_MIN = fminf(max_jump_dx * prof.gap_min_k, GAP_MAX - 10.0f);
    const float STEP_MAX_UP = max_jump_up * prof.step_up_k;
    const float STEP_MAX_DOWN = prof.step_down_px;

    // Параметры платформ (с учётом сложности)
    const float FLOAT_H = prof.float_h;
    const float FLOAT_W_MIN = prof.float_w_min;
    const float FLOAT_W_MAX = prof.float_w_max;
    const float GROUND_W_MIN = prof.ground_w_min;
    const float GROUND_W_MAX = prof.ground_w_max;

    // Стартовая площадка (земля)
    float start_w = 300.0f;
    lvl.platforms[lvl.platformCount++] = (Platform){50, GROUND_Y, (int)start_w, GROUND_HEIGHT, 0, 0, 0, PLATFORM_GROUND};
    lvl.startPlatform = 0;

    // Текущая «путь-платформа», от которой строим следующую
    float cur_x = 50.0f + start_w;
    float cur_y = GROUND_Y; // топ земли

    // Целевая «длина» уровня по шагам с ростом сложности
    int steps = prof.steps_base + difficulty * prof.steps_per_level;
    if (steps > prof.steps_cap) steps = prof.steps_cap;

    for (int i = 0; i < steps && lvl.platformCount + 3 < MAX_PLATFORMS_COUNT; i++) {
        // Распределение категорий зависит от сложности (из профиля)
        int w_ground = prof.w_ground;
        int w_far    = prof.w_far;
        int w_stairs = prof.w_stairs;
        int w_move   = prof.w_move;
        int w_float  = 100 - (w_ground + w_far + w_stairs + w_move);
        if (w_float < 0) w_float = 0;

        int r = rand() % 100;
        int acc = 0;
        int cat_ground = 0, cat_far = 0, cat_stairs = 0, cat_move = 0;
        acc += w_ground; if (r < acc) cat_ground = 1; else {
            acc += w_far; if (r < acc) cat_far = 1; else {
                acc += w_stairs; if (r < acc) cat_stairs = 1; else {
                    acc += w_move; if (r < acc) cat_move = 1; /* иначе float */
                }
            }
        }
        int use_ground = cat_ground;

        // Сгенерировать горизонтальный разрыв в допустимых пределах
        float gap = GAP_MIN + (rand() % (int)fmaxf(1.0f, (GAP_MAX - GAP_MIN)));
        float next_x = cur_x + gap;

        // Вертикальный сдвиг: вверх отрицательный (y меньше), вниз положительный
        float up_limit = (use_ground ? STEP_MAX_UP * 0.5f : STEP_MAX_UP); // на землю сложнее подниматься высоко
        float down_limit = STEP_MAX_DOWN;
        float delta_y = (float)((rand() % (int)(up_limit + down_limit + 1)) - (int)up_limit);
        float next_y = cur_y + delta_y;

        // Ограничим высоту
        if (next_y < 80) next_y = 80; // потолок
        if (next_y > GROUND_Y - 10) next_y = GROUND_Y - 10; // не проваливаться в почву, если плавающая

        if (use_ground) next_y = GROUND_Y; // земля всегда на уровне земли

        // Ширина
        int next_w = use_ground
            ? (int)(GROUND_W_MIN + rand() % (int)fmaxf(1.0f, (GROUND_W_MAX - GROUND_W_MIN)))
            : (int)(FLOAT_W_MIN + rand() % (int)fmaxf(1.0f, (FLOAT_W_MAX - FLOAT_W_MIN)));
        int next_h = use_ground ? GROUND_HEIGHT : (int)FLOAT_H;

        // Запрет перекрытий по X (с небольшим запасом)
        next_x = resolve_x_no_overlap(next_x, next_w, &lvl, 6.0f);

        // Гарантируем достижимость: если вертикальный подъём слишком большой — опустить; если спад слишком большой — подкинуть промежуточную
        if (!use_ground) {
            float rise = cur_y - next_y; // подъём (положительный, если выше)
            if (rise > STEP_MAX_UP) {
                next_y = cur_y - STEP_MAX_UP * 0.9f;
            }
        }

        // Добавляем основную платформу с типом и параметрами движения (если нужно)
        PlatformType ptype = use_ground ? PLATFORM_GROUND : (cat_move ? PLATFORM_MOVING : PLATFORM_FLOATING);
        float pvx = 0, pL = 0, pR = 0;
        if (ptype == PLATFORM_MOVING) {
            // На ранних уровнях плиты шире и медленнее, на поздних — уже и быстрее
            float moving_bonus = (difficulty <= prof.early_levels_threshold) ? prof.moving_bonus_early : 1.0f;
            next_w = (int)(next_w * moving_bonus);
            float travel = fminf(next_w * prof.moving_travel_k, GAP_MAX * 0.6f);
            pL = next_x - travel * 0.5f;
            pR = next_x + next_w + travel * 0.5f;
            float base_speed = prof.moving_speed_base * prof.speed_mult;
            if (difficulty <= prof.early_levels_threshold) base_speed *= 0.75f;
            if (base_speed > prof.moving_speed_cap) base_speed = prof.moving_speed_cap;
            pvx = (rand() % 2 == 0) ? base_speed : -base_speed;
        }
        lvl.platforms[lvl.platformCount++] = (Platform){next_x, next_y, next_w, next_h, pvx, pL, pR, ptype};

        // Паттерн «ступеньки»: теперь реже (10%)
        if (!use_ground && cat_stairs && lvl.platformCount + 3 < MAX_PLATFORMS_COUNT) {
            int stairs = 1 + rand() % 2; // 1-2 ступени
            float step_dx = gap / (stairs + 1);
            float base_y = fminf(cur_y, next_y);
            for (int s = 1; s <= stairs; s++) {
                float sx = cur_x + step_dx * s - 40.0f;
                float sy = base_y - 30.0f * s; // лёгкий подъём
                if (sy < 80) sy = 80;
                int sw = 80 + rand() % 60;
                // Запрет перекрытий по X
                sx = resolve_x_no_overlap(sx, sw, &lvl, 6.0f);
                lvl.platforms[lvl.platformCount++] = (Platform){sx, sy, sw, (int)FLOAT_H, 0, 0, 0, PLATFORM_FLOATING};
            }
        }

        // Промежуточная платформа: только для дальних прыжков (30%)
        if (!use_ground && cat_far && lvl.platformCount + 1 < MAX_PLATFORMS_COUNT) {
            float mid_x = cur_x + (gap * 0.5f);
            float mid_y = fminf(cur_y, next_y) - 30.0f; // немного выше
            if (mid_y < 80) mid_y = 80;
            int mid_w = 80 + rand() % 80;
            mid_x = resolve_x_no_overlap(mid_x, mid_w, &lvl, 6.0f);
            lvl.platforms[lvl.platformCount++] = (Platform){mid_x, mid_y, mid_w, (int)FLOAT_H, 0, 0, 0, PLATFORM_FLOATING};

            // Дополнительная промежуточная иногда отключена для уменьшения ступенек/лестниц
        }

        // Больше не добавляем опоры-земли в разрывах: в гэпы можно упасть

        // Обновляем текущую
        cur_x = next_x + next_w;
        cur_y = next_y;
    }

    // Финишная платформа: недалеко и немного выше/ниже текущей, чтобы точно можно было добраться
    if (lvl.platformCount + 1 < MAX_PLATFORMS_COUNT) {
        float finish_gap = fminf(GAP_MAX * 0.7f, 200.0f);
        float finish_w = 240;
        float finish_x = resolve_x_no_overlap(cur_x + finish_gap, finish_w, &lvl, 6.0f);
        float finish_y = cur_y - fminf(STEP_MAX_UP * 0.5f, 40.0f);
        if (finish_y < 80) finish_y = 80;
        if (finish_y > GROUND_Y - 10) finish_y = GROUND_Y - 10;
        lvl.platforms[lvl.platformCount] = (Platform){finish_x, finish_y, finish_w, (int)FLOAT_H, 0, 0, 0, PLATFORM_FLOATING};
        lvl.finishPlatform = lvl.platformCount;
        lvl.platformCount++;
        // Добавим небольшой участок земли под финишем для визуального завершения
        if (lvl.platformCount < MAX_PLATFORMS_COUNT) {
            lvl.platforms[lvl.platformCount++] = (Platform){finish_x - 40, GROUND_Y, 180, GROUND_HEIGHT, 0, 0, 0, PLATFORM_GROUND};
        }
    } else {
        // Резервный вариант: финиш на земле
        lvl.platforms[lvl.platformCount] = (Platform){cur_x + 120, GROUND_Y, 200, GROUND_HEIGHT, 0, 0, 0, PLATFORM_GROUND};
        lvl.finishPlatform = lvl.platformCount;
        lvl.platformCount++;
    }

    // --- Спавн врагов ---
    int maxEnemies = (difficulty <= 2) ? 1 : (2 + difficulty / 3);
    if (maxEnemies > 8) maxEnemies = 8;
    lvl.enemyCount = 0;
    lvl.enemies = NULL;
    if (maxEnemies > 0) {
        lvl.enemies = (Enemy*)malloc(sizeof(Enemy) * maxEnemies);
        // размещаем врагов на широких неподвижных платформах (земля/плавающие)
        for (int i = 0; i < lvl.platformCount && lvl.enemyCount < maxEnemies; i++) {
            Platform *p = &lvl.platforms[i];
            if (p->type == PLATFORM_MOVING) continue;
            if (p->w < 120) continue;
            // пропустим стартовую платформу и ближайшие к ней сегменты
            if (i == lvl.startPlatform || p->x < lvl.platforms[lvl.startPlatform].x + 300) continue;
            // не на каждую подходящую — разрежаем
            if (rand() % 3 != 0) continue;
            float ex = p->x + p->w * 0.5f - 20.0f; // центр платформы
            float ey = p->y - 40.0f;               // на поверхности
            lvl.enemies[lvl.enemyCount++] = enemy_create(ex, ey);
        }
        // если врагов меньше чем хотели — добьём с земли старт/финиш
        for (int i = 0; i < lvl.platformCount && lvl.enemyCount < maxEnemies; i++) {
            Platform *p = &lvl.platforms[i];
            if (p->y == GROUND_Y) {
                if (p->x < lvl.platforms[lvl.startPlatform].x + 400) continue;
                if (rand() % 3 != 0) continue;
                float ex = p->x + p->w * 0.3f - 20.0f;
                float ey = p->y - 40.0f;
                lvl.enemies[lvl.enemyCount++] = enemy_create(ex, ey);
            }
        }
    }

    return lvl;
}

void level_destroy(Level *lvl) {
    free(lvl->platforms);
    lvl->platforms = NULL;
    lvl->platformCount = 0;
    if (lvl->enemies) {
        free(lvl->enemies);
        lvl->enemies = NULL;
        lvl->enemyCount = 0;
    }
}