#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define SCR_SCALE 20

#define MEM_SIZE 4069
#define STACK_DEPTH 16
#define ENTRY 0x200

typedef struct chip8 {
    uint8_t memory[MEM_SIZE];
    uint16_t stack[STACK_DEPTH];

    uint16_t PC; // program counter
    uint8_t SP; // stack pointer

    uint16_t V[16];
    uint16_t I; // index register

    uint8_t screen[32][64];
} chip8;

const uint8_t sprites[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


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
        uint8_t sprite = c->memory[c->I + row]; // sprite starts at I so...
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
