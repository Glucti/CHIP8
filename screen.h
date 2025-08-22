#ifndef SCREEN_DRAW
#define SCREEN_DRAW

#include <SDL2/SDL.h>
#include <stdio.h>
#include "chip8.h"

void drawScreen(SDL_Renderer* renderer, chip8* c);


void drawScreen(SDL_Renderer* renderer, chip8* c) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
    SDL_RenderClear(renderer); // make the whole screen black

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white

    for (int x = 0; x < WIDTH_SCREEN; x++) {
        for (int y = 0; y < HEIGHT_SCREEN; y++) {
            if (c->screen[x][y]) {
                SDL_Rect rect = {x * SCR_SCALE, y * SCR_SCALE, SCR_SCALE, SCR_SCALE};
                SDL_RenderDrawRect(renderer, &rect);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

#endif


