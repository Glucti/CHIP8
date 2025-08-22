#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "chip8.h"
#include <screen.h>


// 00E0 --> 2 bytes 
// 2F E0 -> 0000 002F  0000 00E0 -> 2FE0 0000 
uint16_t chip8_fetch(chip8* c) {
    uint16_t opcode = (c->memory[c->PC] << 8) | c->memory[c->PC + 1];

    c->PC += 2;

    return opcode;
}

void chip8_init(chip8* c) {
    memset(c, 0, sizeof(chip8));

    c->PC = ENTRY;
    for (int i = 0; i < 80; i++) {
        c->memory[i] = sprites[i];
    }
}

void drawSprite(uint8_t x, uint8_t y, uint8_t n, chip8* c) {
    c->V[0xF] = 0; // set the collision flag to 0
    for (int row = 0; row < n; row++) {
        uint8_t sprite = c->memory[c->I + row]; 
        for (int column = 0; column < 8; column++) {
            if (sprite & (0x80 >> column)) { // 1000 0000 -> 0100 0000 -> etc
                int px = (c->V[x] + column);
                int py = (c->V[y] + row);

                if (c->screen[py][px])
                    c->V[0xF] = 1;

                c->screen[py][px] ^=1; // xor screen
            }
        }
    }
}

// a2e0 -> xxxx xxxx xxxx xxxx
void chip8_decode(chip8* c, uint16_t opcode) {
    switch(opcode & 0xF000) {
        case 0x0000:
            switch(opcode) {
                case 0x00E0:
                    printf("CLS\n");
                    break;
                case 0x00EE:
                    printf("RET\n");
                    break;
                default:
                    printf("SYS add -> ignored\n");
                    break;
            }
            break;
        case 0x1000: {
            uint16_t address = opcode & 0x0FFF;
            printf("JMP to address 0x%03X\n", address);
            c->PC = address;
            break;
        }

        case 0x6000: {
            uint8_t reg = (opcode & 0x0F00) >> 8;
            uint8_t val = (opcode & 0x00FF);

            printf("Register V[%x] -> val %u\n", reg, val);
            c->V[reg] = val;
            break;
        }

        case 0x7000: {
            uint8_t reg = (opcode & 0x0F00) >> 8;
            uint8_t val = (opcode & 0x00FF);

            printf("Adding to reg V[%x] -> val %u\n", reg, val);
            c->V[reg] += val;
            break;
        }

        case 0xA000: {
            uint8_t val = (opcode & 0x0FFF);

            printf("Setting reg I to %03X\n", val);

            c->I = val;
            break;
        }

        case 0XD000: {
            printf("DRW\n");

            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            uint8_t n = (opcode & 0x000F);

            drawSprite(x, y, n, c);
            break;
        }

        default:
            printf("INVALID OPCODE\n");
            break;
    }
}



int main(void) {

    chip8 chip;
    chip8_init(&chip);

    uint8_t opcodes[] = {
        0x00, 0xE0,
        0x1A, 0xBC,
        0x6A, 0x05
    };

    memcpy(&chip.memory[ENTRY], opcodes, sizeof(opcodes));


    for (int i = 0; i < 3; i++) {
        uint16_t fetched = chip8_fetch(&chip);

        chip8_decode(&chip, fetched);
    }

    return 0;
}


// int main() {

//     SDL_Init(SDL_INIT_VIDEO);

//     SDL_Window* window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

//     int running = 1;
//     SDL_Event e;

//     while (running) {
//         while (SDL_PollEvent(&e)) {
//             if (e.type == SDL_QUIT) {
//                 running = 0;
//             }
//         }
//         drawScreen(renderer);
//         SDL_Delay(16);
//     }

//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     SDL_Quit();
//     return 0;
// }
