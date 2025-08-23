// chip8.h
#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

#define SCR_SCALE 20

#define MEM_SIZE 4096
#define STACK_DEPTH 16
#define ENTRY 0x200

#define HEIGHT_SCREEN 32
#define WIDTH_SCREEN 64

typedef struct chip8 {
    uint8_t memory[MEM_SIZE];
    uint16_t stack[STACK_DEPTH];

    uint16_t PC; // program counter
    uint8_t SP; // stack pointer
    uint8_t V[16];
    uint16_t I; // index register
    uint8_t ST; // sound timer 
    uint8_t DT; // delay timer

    uint8_t screen[32][64];
    uint8_t keypad[16];
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

typedef struct {
    uint16_t opcode;
    uint16_t NNN;
    uint8_t NN;
    uint8_t N;
    uint8_t X;
    uint8_t Y;
} instruction_t;

// family handlers
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


#endif