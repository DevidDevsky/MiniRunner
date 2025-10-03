#include <SDL2/SDL.h>
#include "game.h"
#include "player.h"
#include "input.h"
#include "render.h"
#include "config.h"
#include "level.h"
#include "camera.h"
#include "physics.h"

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
        player_update(&player, level.platforms, level.platformCount, dt);
        camera_update(&camera, player.x, player.y);
        render(renderer, &player, &level, &camera);

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
    }

    level_destroy(&level);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}