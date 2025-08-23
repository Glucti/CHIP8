// chip8.c
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "chip8.h"
#include "screen.h"


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

// uint8_t screen[32][64];
void drawSprite(uint8_t x, uint8_t y, uint8_t n, chip8* c) {
    c->V[0xF] = 0; // set the collision flag to 0
    for (int row = 0; row < n; row++) {
        uint8_t sprite = c->memory[c->I + row]; 
        for (int column = 0; column < 8; column++) {
            if (sprite & (0x80 >> column)) { // 1000 0000 -> 0100 0000 -> etc
                int px = (c->V[x] + column) % WIDTH_SCREEN;
                int py = (c->V[y] + row) % HEIGHT_SCREEN;

                if (c->screen[py][px])
                    c->V[0xF] = 1;

                c->screen[py][px] ^=1; // xor screen
            }
        }
    }
}


instruction_t decode_opcode(uint16_t opcode) {

    uint16_t NNN = (opcode & 0xFFF);
    uint8_t NN = (opcode & 0x00FF);
    uint8_t N = (opcode & 0x000F);
    uint8_t X = (opcode & 0x0F00) >> 8;
    uint8_t Y = (opcode & 0x00F0) >> 4;

    instruction_t inst;
    inst.opcode = opcode,
    inst.NNN = NNN;
    inst.NN = NN;
    inst.N = N;
    inst.X = X;
    inst.Y = Y;

    return inst;
}

// a2e0 -> xxxx xxxx xxxx xxxx
void chip8_decode(chip8* c, uint16_t opcode) {

    instruction_t i = decode_opcode(opcode);
    switch(opcode & 0xF000) {
        case 0x0000:
            switch(opcode) {
                case 0x00E0:
                    memset(c->screen, 0, sizeof c->screen);
                    printf("CLS\n");
                    break;
                case 0x00EE: // pop the last address from stack and set the PC to it
                    printf("RET\n");
                    c->SP--;
                    c->PC = c->stack[c->SP];
                    break;
                default:
                    printf("SYS add -> ignored\n");
                    break;
            }
            break;
        case 0x2000: {
            c->stack[c->SP] = c->PC;
            c->SP++;
            c->PC = i.NNN;
            break;
        }
        case 0x1000: {
            c->PC = i.NNN;
            break;
        }
        case 0x3000: {
            if (c->V[i.X] == i.NN) {
                c->PC += 2;
            } 
            break;
        }
        case 0x4000: {
            if (c->V[i.X] != i.NN) {
                c->PC += 2;
            }
            break;
        }
        case 0x5000: {
            if (c->V[i.X] == c->V[i.Y]) {
                c->PC += 2;
            }
            break;
        }
        case 0x9000: {
            if (c->V[i.X] != c->V[i.Y]) {
                c->PC += 2;
            }
            break;
        }
        case 0x6000: {
            c->V[i.X] = i.NN;
            break;
        }
        case 0x7000: {
            c->V[i.X] += i.NN;
            break;
        }
        case 0x8000: {
            switch(i.N) {
                case 0: {
                    c->V[i.X] = c->V[i.Y];
                    break;
                }
                case 1: {
                    c->V[i.X] |= c->V[i.Y];
                    break;
                }
                case 2: {
                    c->V[i.X] &= c->V[i.Y];
                    break;
                }
                case 3: {
                    c->V[i.X] ^= c->V[i.Y];
                    break;
                }
                case 4: {
                    uint16_t sum = c->V[i.X] + c->V[i.Y];
                    c->V[0xF] = (sum > 0xFF);
                    c->V[i.X] = (uint8_t)sum;
                    break;
                }
                case 5: {
                    c->V[0xF] = (c->V[i.X] >= c->V[i.Y]) ? 1 : 0;
                    c->V[i.X] -= c->V[i.Y];
                    break;
                }
                case 6: {
                    c->V[i.X] = c->V[i.Y];
                    c->V[0xF] = c->V[i.Y] & 0x1;
                    c->V[i.X] >>= 1;
                    break;
                }
                case 7: {
                    c->V[0xF] = (c->V[i.Y] >= c->V[i.X]) ? 1 : 0;
                    c->V[i.X] = c->V[i.Y] - c->V[i.X];
                    break;
                }
                case 0xE: {
                    c->V[i.X] = c->V[i.Y];
                    c->V[0xF] = (c->V[i.Y] & 0x80) >> 7;
                    c->V[i.X] <<= 1;
                    break;
                }
            }
            break;
        }
        case 0xA000: {
            c->I = i.NNN;
            break;
        }
        case 0XD000: {
            drawSprite(i.X, i.Y, i.N, c);
            break;
        }
        default:
            printf("INVALID OPCODE\n");
            break;
    }
}

int read_f_into_entry(const char* path, chip8* c) {
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        perror("Failed to open ROM");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    if (size > (MEM_SIZE - ENTRY)) {
        fprintf(stderr, "ROM too big\n");
        fclose(fp);
        return 0;
    }

    fread(&c->memory[ENTRY], 1, size, fp);
    fclose(fp);

    c->PC = ENTRY;
    return 1;
}


int main() {
  chip8 chip;
  chip8_init(&chip);

  // load fonts 
  for (int i = 0; i < 80; i++) chip.memory[i] = sprites[i];

  if (!read_f_into_entry("1-chip8-logo.ch8", &chip)) return 1;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH_SCREEN * SCR_SCALE, HEIGHT_SCREEN * SCR_SCALE, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  int running = 1;
  SDL_Event e;

  while (running) {
      while (SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT) {
              running = 0;
          }
      }

      for (int i = 0; i < 10; i++) {
        uint16_t op = chip8_fetch(&chip);
        chip8_decode(&chip, op);
      }

      drawScreen(renderer, &chip);
      SDL_Delay(16);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
