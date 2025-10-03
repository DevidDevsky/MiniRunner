#include "render.h"
#include "config.h"
#include "platform.h"

void render(SDL_Renderer *renderer, Player *p, Level *lvl, Camera *cam) {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderClear(renderer);

    // земля
    SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
    SDL_Rect ground = {0 - (int)cam->x, GROUND_Y, SCREEN_WIDTH * 2, GROUND_HEIGHT}; // Увеличим ширину земли
    SDL_RenderFillRect(renderer, &ground);

    // платформы
    for (int i = 0; i < lvl->platformCount; i++) {
        Platform* plat = &lvl->platforms[i];
        SDL_Rect plat_rect = {(int)(plat->x - cam->x), (int)(plat->y - cam->y), plat->w, plat->h};
        SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
        SDL_RenderFillRect(renderer, &plat_rect);
    }

    // игрок
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {(int)(p->x - cam->x), (int)(p->y - cam->y), PLAYER_SIZE, PLAYER_SIZE};
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}