#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "chip8.h"
#include <time.h>
#include "screen.h"

// handlers
void op_0_family(chip8*, instruction_t);
void op_1NNN(chip8*, instruction_t);
void op_2NNN(chip8*, instruction_t);
void op_3XNN(chip8*, instruction_t);
void op_4XNN(chip8*, instruction_t);
void op_5XY0(chip8*, instruction_t);
void op_6XNN(chip8*, instruction_t);
void op_7XNN(chip8*, instruction_t);
void op_8_family(chip8*, instruction_t);
void op_9XY0(chip8*, instruction_t);
void op_ANNN(chip8*, instruction_t);
void op_BNNN(chip8*, instruction_t);
void op_CXNN(chip8*, instruction_t);
void op_DXYN(chip8*, instruction_t);
void op_E_family(chip8*, instruction_t);
void op_F_family(chip8*, instruction_t);

void op_8XY0(chip8*, instruction_t);
void op_8XY1(chip8*, instruction_t);
void op_8XY2(chip8*, instruction_t);
void op_8XY3(chip8*, instruction_t);
void op_8XY4(chip8*, instruction_t);
void op_8XY5(chip8*, instruction_t);
void op_8XY6(chip8*, instruction_t);
void op_8XY7(chip8*, instruction_t);
void op_8XYE(chip8*, instruction_t);

void op_EX9E(chip8*, instruction_t);
void op_EXA1(chip8*, instruction_t);
void op_FX07(chip8*, instruction_t);
void op_FX0A(chip8*, instruction_t);
void op_FX15(chip8*, instruction_t);
void op_FX18(chip8*, instruction_t);
void op_FX1E(chip8*, instruction_t);
void op_FX29(chip8*, instruction_t);
void op_FX33(chip8*, instruction_t);
void op_FX55(chip8*, instruction_t);
void op_FX65(chip8*, instruction_t);

void chip8_init(chip8* c) {
    memset(c, 0, sizeof(chip8));
    memset(c->keypad, 0, sizeof c->keypad);

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

// opcode table
typedef void (*op_handler)(chip8* c, instruction_t i);

op_handler main_table[16] = {
    op_0_family,  
    op_1NNN,      
    op_2NNN,      
    op_3XNN,      
    op_4XNN,      
    op_5XY0,      
    op_6XNN,      
    op_7XNN,      
    op_8_family,  
    op_9XY0,      
    op_ANNN,      
    op_BNNN,      
    op_CXNN,      
    op_DXYN,      
    op_E_family,  
    op_F_family   
};

void op_0_family(chip8* c, instruction_t i) {
    switch (i.NN) {
        case 0xE0: {
            memset(c->screen, 0, sizeof c->screen);
            break;
        }
        case 0xEE: {
            if (c->SP == 0) {printf("STACK UNDERFLOW\n"); return;} // DEBUG
            c->SP--;
            c->PC = c->stack[c->SP];
            break;
        }
        default:
            break;
    }
}
void op_1NNN(chip8* c, instruction_t i) {
    c->PC = i.NNN;
}
void op_2NNN(chip8* c, instruction_t i) {
    if (c->SP >= STACK_DEPTH) { printf("STACK OVERFLOW\n"); return;} // DEBUG
    c->stack[c->SP] = c->PC;
    c->SP++;
    c->PC = i.NNN;
}
void op_3XNN(chip8* c, instruction_t i) {
    if (c->V[i.X] == i.NN) 
        c->PC += 2;
}
void op_4XNN(chip8* c, instruction_t i) {
    if (c->V[i.X] != i.NN)
        c->PC += 2;
}
void op_5XY0(chip8* c, instruction_t i) {
    if ((i.opcode & 0x000f) != 0) {
        return;
    }
    if (c->V[i.X] == c->V[i.Y]) {
        c->PC += 2;
    }
}
void op_6XNN(chip8* c, instruction_t i) {
    c->V[i.X] = i.NN;
}
void op_7XNN(chip8* c, instruction_t i) {
    c->V[i.X] += i.NN;
}
void op_8_family(chip8* c, instruction_t i) {
    static op_handler table[16] = {
        op_8XY0,
        op_8XY1,
        op_8XY2,
        op_8XY3,
        op_8XY4,
        op_8XY5,
        op_8XY6,
        op_8XY7,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        op_8XYE,
        NULL
    };

    op_handler fn = table[i.N];
    if (fn) fn(c, i);
    else printf("INVALID 8XY%X\n", i.N);
}
void op_8XY0(chip8* c, instruction_t i) {
    c->V[i.X] = c->V[i.Y];
}
void op_8XY1(chip8* c, instruction_t i) {
    c->V[i.X] |= c->V[i.Y];
}
void op_8XY2(chip8* c, instruction_t i) {
    c->V[i.X] &= c->V[i.Y];
}
void op_8XY3(chip8* c, instruction_t i) {

    c->V[i.X] ^= c->V[i.Y];
}
void op_8XY4(chip8* c, instruction_t i) {
    uint16_t sum = c->V[i.X] + c->V[i.Y];
    c->V[0xF] = (sum > 0xFF);
    c->V[i.X] = (uint8_t)sum;
}
void op_8XY5(chip8* c, instruction_t i) {
    c->V[0xF] = (c->V[i.X] >= c->V[i.Y]) ? 1 : 0;
    c->V[i.X] -= c->V[i.Y];
}
void op_8XY6(chip8* c, instruction_t i) {
    c->V[i.X] = c->V[i.Y];
    c->V[0xF] = c->V[i.Y] & 0x1;
    c->V[i.X] >>= 1;
}
void op_8XY7(chip8* c, instruction_t i) {
    c->V[0xF] = (c->V[i.Y] >= c->V[i.X]) ? 1 : 0;
    c->V[i.X] = c->V[i.Y] - c->V[i.X];
}
void op_8XYE(chip8* c, instruction_t i) {
    c->V[i.X] = c->V[i.Y];
    c->V[0xF] = (c->V[i.Y] & 0x80) >> 7;
    c->V[i.X] <<= 1;
}
void op_9XY0(chip8* c, instruction_t i) {
    if ((i.opcode & 0x000f) != 0) {
        return;
    }
    if (c->V[i.X] != c->V[i.Y]) {
        c->PC += 2;
    }
}
void op_ANNN(chip8* c, instruction_t i) {
    c->I = i.NNN;
}
void op_BNNN(chip8* c, instruction_t i) {
    uint16_t target = i.NNN + c->V[0];
    if (target < ENTRY || target >= MEM_SIZE) {
        printf("BNNN to %03X out of range\n", target);
    }
}

void op_CXNN(chip8* c, instruction_t i) {

    uint8_t rnd = rand() & 0xFF;
    c->V[i.X] = rnd & i.NN;
}      
void op_DXYN(chip8* c, instruction_t i) {
    drawSprite(i.X, i.Y, i.N, c);
}   
void op_E_family(chip8* c, instruction_t i) {
    switch (i.NN) {
        case 0x9E: op_EX9E(c, i); break;
        case 0xA1: op_EXA1(c, i); break;
        default: printf("INVALID EX%02X\n", i.NN); break;
    }
}
void op_EX9E(chip8* c, instruction_t i) {
    printf("EXA1: V%X=%u keypad[%u]=%u\n", i.X, c->V[i.X], c->V[i.X] & 0xF, c->keypad[c->V[i.X] & 0xF]);
    if (c->keypad[ c->V[i.X] & 0xF]) {
        c->PC += 2;
    }
}

void op_EXA1(chip8* c, instruction_t i) {
    printf("EX9E: V%X=%u keypad[%u]=%u\n", i.X, c->V[i.X], c->V[i.X] & 0xF, c->keypad[c->V[i.X] & 0xF]);
    if (!c->keypad[ c->V[i.X] & 0xF ]) {
        c->PC += 2;
    }
}

void op_F_family(chip8* c, instruction_t i) {
    switch (i.NN) {
        case 0x07: op_FX07(c, i); break;
        case 0x15: op_FX15(c, i); break;
        case 0x18: op_FX18(c, i); break;
        case 0x1E: op_FX1E(c, i); break;
        case 0x0A: op_FX0A(c, i); break;
        case 0x29: op_FX29(c, i); break;
        case 0x33: op_FX33(c, i); break;
        case 0x55: op_FX55(c, i); break;
        case 0x65: op_FX65(c, i); break;
        default: printf("INVALID FX%02X\n", i.NN); break;
    }
}
void op_FX07(chip8* c, instruction_t i) {
    c->V[i.X] = c->DT;
} 
void op_FX15(chip8* c, instruction_t i) {
    c->DT = c->V[i.X];
}
void op_FX18(chip8* c, instruction_t i) {
    c->ST = c->V[i.X];
}
void op_FX1E(chip8* c, instruction_t i) {
    c->I += c->V[i.X];
}
void op_FX0A(chip8* c, instruction_t i) {
    for (uint8_t k =0; k < 16; k++) {
        if (c->keypad[k]) {
            c->V[i.X] = k;
            return ;
        }
    }
    c->PC -= 2;
}
void op_FX29(chip8* c, instruction_t i) {
    c->I = c->V[i.X] * 5;
}
void op_FX33(chip8* c, instruction_t i) {
    uint8_t val = c->V[i.X];
    uint8_t hunnids = (val / 100) % 10;
    uint8_t tens = (val / 10) % 10;
    uint8_t ones = val % 10;

    c->memory[c->I] = hunnids;
    c->memory[c->I + 1] = tens;
    c->memory[c->I + 2] = ones;
}
void op_FX55(chip8* c, instruction_t i) {
    for (int r = 0; r <= i.X; r++) {
        if (c->I + r < MEM_SIZE)
            c->memory[c->I + r] = c->V[r];
    }
}
void op_FX65(chip8* c, instruction_t i) {
    for (int r = 0; r <= i.X; r++) {
        if (c->I + r < MEM_SIZE) {
            c->V[r] = c->memory[c->I + r];
        }
    }
}

void chip8_fetch_decode(chip8* c) {
    uint16_t opcode = (c->memory[c->PC] << 8) | c->memory[c->PC + 1];

    c->PC += 2;

    instruction_t inst = decode_opcode(opcode);

    op_handler fn = main_table[(opcode & 0xF000) >> 12];
    if (fn) {
        fn(c, inst);
    } else {
        printf("INVALID OPCODE %04X at PC=%03X\n", opcode, c->PC - 2);
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
void chip8_timers(chip8* c) {
    if (c->DT > 0) {
        c->DT--;
    }
    if (c->ST > 0) {
        c->ST--;
        // beep witih SDL audio
    }
}


int main() {
  chip8 chip;
  chip8_init(&chip);
  srand((unsigned)time(NULL));


  if (!read_f_into_entry("caveexplorer.ch8", &chip)) return 1;

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH_SCREEN * SCR_SCALE, HEIGHT_SCREEN * SCR_SCALE, SDL_WINDOW_SHOWN);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  bool running = true;
  SDL_Event e;
  uint32_t last_timer = SDL_GetTicks();

  while (running) {
      handle_input(&chip, &running);

      for (int i = 0; i < 10; i++) {
        chip8_fetch_decode(&chip);
      }
      uint32_t now = SDL_GetTicks();
      if (now - last_timer >= 16) {
        chip8_timers(&chip);
        last_timer = now;
      }

      drawScreen(renderer, &chip);
      SDL_Delay(1);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
