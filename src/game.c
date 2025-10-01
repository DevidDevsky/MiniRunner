#include <SDL2/SDL.h>
#include "game.h"
#include "player.h"
#include "input.h"
#include "render.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void game_loop() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Runner",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Player player = player_create(100, 550);

    int running = 1;
    while (running) {
        running = handle_input(&player);
        player_update(&player);
        render(renderer, &player);
        SDL_Delay(16); // ~60 fps
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}