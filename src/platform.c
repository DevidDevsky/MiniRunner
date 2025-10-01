#include "platform.h"

void platform_render(SDL_Renderer *renderer, Platform *p) {
    SDL_Rect rect = {(int)p->x, (int)p->y, (int)p->w, (int)p->h};
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // коричневый
    SDL_RenderFillRect(renderer, &rect);
}