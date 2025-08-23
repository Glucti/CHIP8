#ifndef SCREEN_DRAW
#define SCREEN_DRAW

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include "chip8.h"


void handle_input(chip8* c, bool* running) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *running = 0;
                break;
            
            case SDL_KEYDOWN: {


                if (event.key.repeat) break;

                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        *running = 0;
                        break;

                    case SDLK_1: c->keypad[0x1] = true; break;
                    case SDLK_2: c->keypad[0x2] = true; break;
                    case SDLK_3: c->keypad[0x3] = true; break;
                    case SDLK_4: c->keypad[0xC] = true; break;

                    case SDLK_q: c->keypad[0x4] = true; break;
                    case SDLK_w: c->keypad[0x5] = true; break;
                    case SDLK_e: c->keypad[0x6] = true; break;
                    case SDLK_r: c->keypad[0xD] = true; break;

                    case SDLK_a: c->keypad[0x7] = true; break;
                    case SDLK_s: c->keypad[0x8] = true; break;
                    case SDLK_d: c->keypad[0x9] = true; break;
                    case SDLK_f: c->keypad[0xE] = true; break;

                    case SDLK_z: c->keypad[0xA] = true; break;
                    case SDLK_x: c->keypad[0x0] = true; break;
                    case SDLK_c: c->keypad[0xB] = true; break;
                    case SDLK_v: c->keypad[0xF] = true; break;

                    default: break;
                }
                break;
            }
            
            case SDL_KEYUP: {
                switch (event.key.keysym.sym) {

                    case SDLK_1: c->keypad[0x1] = false; break;
                    case SDLK_2: c->keypad[0x2] = false; break;
                    case SDLK_3: c->keypad[0x3] = false; break;
                    case SDLK_4: c->keypad[0xC] = false; break;

                    case SDLK_q: c->keypad[0x4] = false; break;
                    case SDLK_w: c->keypad[0x5] = false; break;
                    case SDLK_e: c->keypad[0x6] = false; break;
                    case SDLK_r: c->keypad[0xD] = false; break;

                    case SDLK_a: c->keypad[0x7] = false; break;
                    case SDLK_s: c->keypad[0x8] = false; break;
                    case SDLK_d: c->keypad[0x9] = false; break;
                    case SDLK_f: c->keypad[0xE] = false; break;

                    case SDLK_z: c->keypad[0xA] = false; break;
                    case SDLK_x: c->keypad[0x0] = false; break;
                    case SDLK_c: c->keypad[0xB] = false; break;
                    case SDLK_v: c->keypad[0xF] = false; break;

                    default: break;
                }
                break;
            }


        }
    }
}


void drawScreen(SDL_Renderer* renderer, chip8* c) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
    SDL_RenderClear(renderer); // make the whole screen black

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white

    for (int y = 0; y < HEIGHT_SCREEN; y++) {
        for (int x = 0; x <WIDTH_SCREEN; x++) {
            if (c->screen[y][x]) {
                SDL_Rect rect = {x * SCR_SCALE, y * SCR_SCALE, SCR_SCALE, SCR_SCALE};
                SDL_RenderDrawRect(renderer, &rect);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

#endif


