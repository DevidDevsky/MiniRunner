#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>

typedef enum {
    PLATFORM_GROUND = 0,
    PLATFORM_FLOATING = 1,
    PLATFORM_MOVING = 2,
    PLATFORM_CRUMBLE = 3
} PlatformType;

typedef struct {
    float x, y;
    float w, h;
    // Movement params (used when type == PLATFORM_MOVING)
    float vx;       // current horizontal speed
    float rangeL;   // left bound of movement range
    float rangeR;   // right bound of movement range
    PlatformType type;
    // Crumble behavior (used when type == PLATFORM_CRUMBLE)
    int crumbles;         // 1 if crumble has been triggered
    float crumbleDelay;   // seconds before disappearing after trigger
    float crumbleTimer;   // countdown in seconds
} Platform;

void platform_render(SDL_Renderer *renderer, Platform *p);

#endif