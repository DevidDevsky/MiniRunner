#include <SDL2/SDL.h>
#include "game.h"
#include "player.h"
#include "input.h"
#include "render.h"
#include "config.h"

void game_loop() {
    Uint32 window_flags = 0;
    if (FULLSCREEN) {
        window_flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Runner",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, window_flags);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Player player = player_create(100, GROUND_Y - PLAYER_SIZE);

    int running = 1;
    while (running) {
        running = handle_input(&player, window);
        player_update(&player);
        render(renderer, &player);
        SDL_Delay(16); // временно FPS фиксируем
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}