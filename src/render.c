#include "render.h"
#include "config.h"

void render(SDL_Renderer *renderer, Player *p) {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderClear(renderer);

    // земля
    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    SDL_Rect ground = {0, GROUND_Y, SCREEN_WIDTH, GROUND_HEIGHT};
    SDL_RenderFillRect(renderer, &ground);

    // игрок
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {(int)p->x, (int)p->y, PLAYER_SIZE, PLAYER_SIZE};
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}