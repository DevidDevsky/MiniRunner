#include <SDL2/SDL.h>
#include "game.h"
#include "player.h"
#include "input.h"
#include "render.h"
#include "config.h"
#include "level.h"
#include "camera.h"
#include "physics.h"
#include "enemy.h"

// Находим индекс ближайшей платформы СЗАДИ относительно позиции x
static int find_respawn_platform(Level *level, float x) {
    int idx = level->startPlatform;
    float bestX = -1e9f;
    for (int i = 0; i < level->platformCount; i++) {
        Platform *p = &level->platforms[i];
        float px = p->x + p->w * 0.5f;
        if (px <= x && px > bestX) {
            bestX = px;
            idx = i;
        }
    }
    return idx;
}

void game_loop() {
    Uint32 window_flags = FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Runner",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    int difficulty = 1;
    Level level = level_create(difficulty);

    Platform start_platform = level.platforms[level.startPlatform];
    float player_start_x = start_platform.x + (start_platform.w / 2) - (PLAYER_SIZE / 2);
    float player_start_y = start_platform.y - PLAYER_SIZE;
    Player player = player_create(player_start_x, player_start_y);

    Camera camera = {0, 0};

    int running = 1;
    Uint32 lastTick = SDL_GetTicks();

    while (running) {
        Uint32 currentTick = SDL_GetTicks();
        float dt = (currentTick - lastTick) / 16.0f; // нормализация (1.0 ~ 60 FPS)
        if (dt > 2.0f) dt = 2.0f; // защита от лагов
        lastTick = currentTick;

        running = handle_input(&player, window);
        platforms_update_all(level.platforms, level.platformCount, dt);
        // обновление врагов
        if (level.enemies && level.enemyCount > 0) {
            enemies_update_all(level.enemies, level.enemyCount, level.platforms, level.platformCount, dt);
        }
        // таймеры: неуязвимость и блок управления после респауна
        if (player.invFrames > 0) player.invFrames -= 1;
        if (player.respawnLockFrames > 0) player.respawnLockFrames -= 1;
        player_update(&player, level.platforms, level.platformCount, dt);
        camera_update(&camera, player.x, player.y);
        // HUD + рендер с миганием при неуязвимости
        {
            SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
            SDL_RenderClear(renderer);

            // платформы и враги/игрок рендерим через существующий render()
            // но реализуем мигание игрока: будем рендерить сцену сами
            // Сначала платформы и враги
            for (int i = 0; i < level.platformCount; i++) {
                Platform* plat = &level.platforms[i];
                SDL_Rect plat_rect = {(int)(plat->x - camera.x), (int)(plat->y - camera.y), plat->w, plat->h};
                if (plat->type == PLATFORM_MOVING) SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255);
                else if (plat->y == GROUND_Y && plat->h == GROUND_HEIGHT) SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
                else SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
                SDL_RenderFillRect(renderer, &plat_rect);
            }
            if (level.enemies && level.enemyCount > 0) {
                SDL_SetRenderDrawColor(renderer, 160, 32, 240, 255);
                for (int i = 0; i < level.enemyCount; i++) {
                    Enemy *e = &level.enemies[i]; if (!e->alive) continue;
                    SDL_Rect er = {(int)(e->x - camera.x), (int)(e->y - camera.y), e->w, e->h};
                    SDL_RenderFillRect(renderer, &er);
                }
            }
            // Игрок (мигаем при invFrames)
            int drawPlayer = 1;
            if (player.invFrames > 0) {
                if (((player.invFrames / 4) % 2) == 0) drawPlayer = 0; // пропуск кадра
            }
            if (drawPlayer) {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_Rect pr = {(int)(player.x - camera.x), (int)(player.y - camera.y), PLAYER_SIZE, PLAYER_SIZE};
                SDL_RenderFillRect(renderer, &pr);
            }

            // HUD: жизни (сердечки) и уровень
            int heartSize = 12; int margin = 6;
            for (int i = 0; i < player.lives; i++) {
                SDL_SetRenderDrawColor(renderer, 220, 20, 60, 255); // crimson
                SDL_Rect heart = {10 + i*(heartSize+margin), 10, heartSize, heartSize};
                SDL_RenderFillRect(renderer, &heart);
            }
            // Плашка уровня/сложности
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
            SDL_Rect hud = {10, 28 + heartSize, 120, 14};
            SDL_RenderFillRect(renderer, &hud);
            // Заполняем бар пропорционально сложности (визуальная индикация)
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
            int w = (int)( (level.difficulty % 10) * (120.0f/10.0f) );
            if (w < 8) w = 8;
            SDL_Rect bar = {10, 28 + heartSize, w, 14};
            SDL_RenderFillRect(renderer, &bar);

            SDL_RenderPresent(renderer);
        }

        // Проверка достижения финиша
        Platform finish_platform = level.platforms[level.finishPlatform];
        if (player.x > finish_platform.x && player.x < finish_platform.x + finish_platform.w) {
            difficulty++;
            level_destroy(&level);
            level = level_create(difficulty);

            Platform start_platform = level.platforms[level.startPlatform];
            player.x = start_platform.x + (start_platform.w / 2) - (PLAYER_SIZE / 2);
            player.y = start_platform.y - PLAYER_SIZE;
            player.vx = 0;
            player.vy = 0;
        }

        // Столкновение игрока с врагами: стомп или -1 жизнь и респаун с i-frames (игнор урона во время i-frames)
        if (level.enemies && level.enemyCount > 0) {
            SDL_Rect pr = {(int)player.x, (int)player.y, PLAYER_SIZE, PLAYER_SIZE};
            for (int i = 0; i < level.enemyCount; i++) {
                Enemy *e = &level.enemies[i];
                if (!e->alive) continue;
                SDL_Rect er = {(int)e->x, (int)e->y, e->w, e->h};
                if (SDL_HasIntersection(&pr, &er)) {
                    // Проверим "стомп": игрок падал, и его нижняя грань в прошлом кадре была выше верха врага
                    float player_prev_bottom = player.lastY + PLAYER_SIZE;
                    if (player.vy > 0 && player_prev_bottom <= e->y + 4) {
                        // убиваем врага и подпрыгиваем коротко
                        e->alive = 0;
                        player.vy = JUMP_FORCE * 0.6f;
                        player.invFrames = 10; // короткий буфер
                    } else if (player.invFrames == 0) {
                        // получаем урон
                        player.lives -= 1;
                        if (player.lives < 1) {
                            // Game Over: сброс на 1 уровень и восстановление жизней
                            difficulty = 1;
                            level_destroy(&level);
                            level = level_create(difficulty);
                            Platform sp = level.platforms[level.startPlatform];
                            player.x = sp.x + (sp.w / 2) - (PLAYER_SIZE / 2);
                            player.y = sp.y - PLAYER_SIZE;
                            player.vx = 0; player.vy = 0; player.lives = 3; player.invFrames = 0; player.respawnLockFrames = 0;
                        } else {
                            int respIdx = find_respawn_platform(&level, player.x);
                            Platform rp = level.platforms[respIdx];
                            player.x = rp.x + (rp.w / 2) - (PLAYER_SIZE / 2);
                            player.y = rp.y - PLAYER_SIZE;
                            player.vx = 0; player.vy = JUMP_FORCE * 0.3f; // небольшой отскок
                            player.invFrames = 180; // ~3 сек при 60 FPS
                            player.respawnLockFrames = 30; // ~0.5 сек блок
                        }
                        break;
                    }
                }
            }
        }

        // Падение игрока в пропасть — -1 жизнь и респаун на ближайшей платформе сзади
        if (player.y > SCREEN_HEIGHT + 200) {
            player.lives -= 1;
            if (player.lives < 1) {
                // Game Over: сброс прогресса
                difficulty = 1;
                level_destroy(&level);
                level = level_create(difficulty);
                Platform sp = level.platforms[level.startPlatform];
                player.x = sp.x + (sp.w / 2) - (PLAYER_SIZE / 2);
                player.y = sp.y - PLAYER_SIZE;
                player.vx = 0; player.vy = 0; player.lives = 3; player.invFrames = 0; player.respawnLockFrames = 0;
            } else {
                int respIdx = find_respawn_platform(&level, player.x);
                Platform rp = level.platforms[respIdx];
                player.x = rp.x + (rp.w / 2) - (PLAYER_SIZE / 2);
                player.y = rp.y - PLAYER_SIZE;
                player.vx = 0; player.vy = JUMP_FORCE * 0.3f; player.invFrames = 180; // ~3 сек при 60 FPS
                player.respawnLockFrames = 30; // ~0.5 сек блок
            }
        }
    }

    level_destroy(&level);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}