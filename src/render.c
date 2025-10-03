#include "render.h"
#include "config.h"
#include "platform.h"

void render(SDL_Renderer *renderer, Player *p, Level *lvl, Camera *cam) {
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);
    SDL_RenderClear(renderer);

    // платформы
    for (int i = 0; i < lvl->platformCount; i++) {
        Platform* plat = &lvl->platforms[i];
        SDL_Rect plat_rect = {(int)(plat->x - cam->x), (int)(plat->y - cam->y), plat->w, plat->h};
        // Отрисуем по типам: земля зелёная, платформы коричневые, движущиеся синие
        if (plat->type == PLATFORM_MOVING) {
            SDL_SetRenderDrawColor(renderer, 70, 130, 180, 255); // steel blue
        } else if (plat->y == GROUND_Y && plat->h == GROUND_HEIGHT) {
            SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);
        }
        SDL_RenderFillRect(renderer, &plat_rect);
    }

    // враги
    if (lvl->enemies && lvl->enemyCount > 0) {
        SDL_SetRenderDrawColor(renderer, 160, 32, 240, 255); // purple
        for (int i = 0; i < lvl->enemyCount; i++) {
            Enemy *e = &lvl->enemies[i];
            if (!e->alive) continue;
            SDL_Rect er = {(int)(e->x - cam->x), (int)(e->y - cam->y), e->w, e->h};
            SDL_RenderFillRect(renderer, &er);
        }
    }

    // игрок
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect rect = {(int)(p->x - cam->x), (int)(p->y - cam->y), PLAYER_SIZE, PLAYER_SIZE};
    SDL_RenderFillRect(renderer, &rect);

    SDL_RenderPresent(renderer);
}