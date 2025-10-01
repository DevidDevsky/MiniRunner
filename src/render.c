#include "render.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

void render(SDL_Renderer *renderer, Player *p) {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderClear(renderer);

    // земля
    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    SDL_Rect ground = {0, 600 - 50, SCREEN_WIDTH, 50};
    SDL_RenderFillRect(renderer, &ground);

    // игрок
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {(int)p->x, (int)p->y, 50, 50};
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}