#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>

typedef struct {
    float x, y;
    float w, h;
} Platform;

void platform_render(SDL_Renderer *renderer, Platform *p);

#endif