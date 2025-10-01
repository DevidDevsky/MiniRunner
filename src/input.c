#include "input.h"

int handle_input(Player *p, SDL_Window *window) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) return 0;
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) return 0;
            if (event.key.keysym.sym == SDLK_SPACE) player_jump(p);

            // переключение fullscreen (F11)
            if (event.key.keysym.sym == SDLK_F11) {
                Uint32 flags = SDL_GetWindowFlags(window);
                if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
                    SDL_SetWindowFullscreen(window, 0);
                } else {
                    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                }
            }
        }
    }

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    float dir = 0;
    if (state[SDL_SCANCODE_LEFT])  dir = -1;
    if (state[SDL_SCANCODE_RIGHT]) dir = 1;
    player_move(p, dir);

    return 1;
}