#include <SDL2/SDL.h>
#include "game.h"
#include "player.h"
#include "input.h"
#include "render.h"
#include "config.h"
#include "level.h"

void game_loop() {
    Uint32 window_flags = FULLSCREEN ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Runner",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Player player = player_create(100, GROUND_Y - PLAYER_SIZE);
    Level level = level_create();

    int running = 1;
    Uint32 lastTick = SDL_GetTicks();

    while (running) {
        Uint32 currentTick = SDL_GetTicks();
        float dt = (currentTick - lastTick) / 16.0f; // нормализация (1.0 ~ 60 FPS)
        if (dt > 2.0f) dt = 2.0f; // защита от лагов
        lastTick = currentTick;

        running = handle_input(&player, window);
        player_update(&player, level.platforms, level.platformCount, dt);
        render(renderer, &player, &level);
    }

    level_destroy(&level);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}