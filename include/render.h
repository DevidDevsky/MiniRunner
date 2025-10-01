#ifndef RENDER_H
#define RENDER_H

#include <SDL2/SDL.h>
#include "player.h"
#include "platform.h"
#include "level.h"

void render(SDL_Renderer *renderer, Player *p, Level *lvl);

#endif