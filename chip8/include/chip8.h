#ifndef CHIP8_H_
#define CHIP8_H_

#include <stdint.h>
#include "raylib.h"

#define FONTSET_START_ADDRESS 50
#define FONT_SIZE 5

#define CHIP8_SCREEN_WIDTH 64
#define CHIP8_SCREEN_HEIGHT 32

#define ENTRY_POINT 0x200

/* Macro to extract a specific nibble (0-7) from a hex value */
#define GET_NIBBLE(hexValue, index) (((hexValue) >> ((index) * 4)) & 0xF)

typedef struct chip8 {
  uint8_t registers[16];
  uint8_t memory[4096];
  uint16_t pc;
  uint16_t ir;
  uint8_t graphics[CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH];
  uint16_t stack[16];
  uint8_t sp;
  uint8_t delay_timer;
  uint8_t sound_timer;
  uint16_t op_code;
  uint8_t keypad[16];
  uint8_t *rom_name;
} Chip8;

typedef enum {
  V0 = 0,
  V1,
  V2,
  V3,
  V4,
  V5,
  V6,
  V7,
  V8,
  V9,
  VA,
  VB,
  VC,
  VD,
  VE,
  VF
} GeneralPurposeRegs;

typedef void (*function) (Chip8 *self) ;

int chip8_init(Chip8 *self);
int chip8_load_rom(Chip8 *self, char *usr_rom_name);
void chip8_parse_code(Chip8 *self);
void chip8_inst_emulate(Chip8 *self);

#endif // CHIP8_H_
