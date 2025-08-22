#include <SDL2/SDL.h>
#include <stdio.h>

#define SCALE 10
#define WIDTH  (64 * SCALE)
#define HEIGHT (32 * SCALE)

uint8_t screen[WIDTH][HEIGHT];

void drawScreen(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // black
    SDL_RenderClear(renderer); // make the whole screen black

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            if (screen[x][y]) {
                SDL_Rect rect = {x * SCALE, y * SCALE, SCALE, SCALE};
                SDL_RenderDrawRect(renderer, &rect);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }

    SDL_RenderPresent(renderer);
}


int main() {

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    screen[0][0] = 1;
    screen[10][10] = 1;
    screen[20][30] = 1;
    screen[60][31] = 1;

    int running = 1;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }
        drawScreen(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

}
