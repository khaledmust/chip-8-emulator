#include "chip8.h"
#include "raylib.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const uint8_t font[] = {
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

int chip8_init(Chip8 *self) {
  /* Initialize the Chip8 structure with 0. */
  /* TODO Check is this step is necessary. */
  memset(self, 0, sizeof(*self));

  /* Load the font into RAM at starting address of 0x000.
   * It was stated in the documentation that we could load the font at any
   * location starting from 0x0000, but, it became a convention to load it at
   * address 0x0050.
   */
  memcpy(&self->memory[50], font, sizeof(font));

  for (int i = 50; i <= 80; i++) {
    printf("Font value: 0x%02x\n", self->memory[i]);
  }

  /* Load initial value of the PC. */
  self->pc = ENTRY_POINT;
  printf("The PC is set at the entry point: 0x%04x\n", self->pc);

  return 0;
}

int chip8_load_rom(Chip8 *self, char *usr_rom_name) {

  /* Set the ROM name to the user's input. */
  self->rom_name = (uint8_t *)usr_rom_name;
  printf("The loaded rom is %s\n", usr_rom_name);

  /* Open ROM file. */
  FILE *rom = fopen((const char *)self->rom_name, "rb");

  /* Check if rom is opened. */
  if (!rom) {
    fprintf(stderr, "ROM file %s in invalid or doesn't exist!\n",
            self->rom_name);
    return false;
  }

  fseek(rom, 0, SEEK_END);

  long int rom_size = ftell(rom);
  long int max_size = sizeof(self->memory) - ENTRY_POINT;

  rewind(rom);

  if (rom_size > max_size) {
    fprintf(stderr,
            "ROM file %s is too large, and can't fit in memory. %zu is the max "
            "size, while the rom is %zu bytes.\n",
            self->rom_name, max_size, rom_size);

    return 1;
  }

  fread(&(self->memory[ENTRY_POINT]), rom_size, 1, rom);

  printf("Dumping memory...\n");
  for (int i = ENTRY_POINT; i < ENTRY_POINT + rom_size; i++) {
    if ((i - 0x200) % 15 == 0 && i > 0x200) {
      printf("\n");
    }
    printf("0x%02x\t", self->memory[i] & 0xff);
  }
  printf("\n");

  fclose(rom);

  return 0;
}

/**
 * @brief Parses 16-bits Opcode from the memory.
 *
 * The function first parses 8-bits of opcode and then increment the PC to point
 * to the next 8-bits opcode which is then ORed to the previous one to form the
 * 16-bits insturction.
 *
 * @param self A pointer to the Chip8 object.
 */
void chip8_parse_code(Chip8 *self) {
  uint16_t old_pc = self->pc;
  self->op_code = self->memory[self->pc] << 8;
  self->pc++;
  self->op_code |= self->memory[self->pc];
  self->pc++;

  printf("Address: 0x%04x, opcode: 0x%04x\n", old_pc, self->op_code);

  if (self->op_code == 0x0000) {
    self->pc = ENTRY_POINT;
  }
}

void chip8_deinit(Chip8 *self) {}

/**
 * @brief Clear the display.
 *
 * The function uses memset() to zero all the graphics array.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_00e0(Chip8 *self) {
  memset(self->graphics, 0, sizeof(self->graphics));
  printf("Cleared the display.\n");
}

/**
 * @brief Return from a subroutine.
 *
 * This is done by first subtracting the SP, as it's currently pointing to the
 * first free address that will be pushed to. And, then loading the the address
 * stored there to the PC.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_00ee(Chip8 *self) {
  self->sp--;
  /* Restoring context. */
  self->pc = self->stack[self->sp];

  printf("Return from subroutine to address 0x%04x.\n", self->pc);
}

/**
 * @brief Calls the specified 0xxx intruction.
 *
 * The function filters the 0xxx opcode by extracting the low nibble and calls
 * the speicified intruction accordingly.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_extract_and_call_0xxx(Chip8 *self) {
  uint8_t low_nibble = GET_NIBBLE(self->op_code, 0);

  switch (low_nibble) {
  case 0x0:
    Chip8_OP_00e0(self);
    break;
  case 0xe:
    Chip8_OP_00ee(self);
    break;
  }
}

/**
 * @brief Jump to the location nnn.
 *
 * The interpreter sets the program counter (PC) to nnn.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_1nnn(Chip8 *self) {
  uint16_t jump_loc = self->op_code & 0x0FFF;

  /* Point the the PC to the jump locaton. */
  self->pc = jump_loc;

  printf("Jump to address 0x%04x\n", jump_loc);
}

/**
 * @brief  Call subroutine at nnn.
 *
 * The interpreter increments the stack pointer, then puts the current PC on the
 * top of the stack. The PC is then set to nnn.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_2nnn(Chip8 *self) {
  /* Push the current PC to the stack. */
  self->stack[self->sp] = self->pc;

  /* Increment the stack pointer (SP). */
  self->sp++;

  /* Set the PC to nnn. */
  self->pc = self->op_code & 0x0FFF;

  printf("Jump to 0x%04x\n", self->pc);
}

/**
 * @brief Skip next instruction if Vx = kk.
 *
 * The interpreter compares register Vx to kk, and if they are equal, increments
 * the program counter by 2.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_3xkk(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t kk = self->op_code & 0x00FF;

  if (self->registers[Vx] == kk) {
    self->pc += 2;
  }

  printf("Checking if Vx = kk, 0x%04x 0x%04x, skip next instruction if true.\n", self->registers[Vx], kk);
}

/**
 * @brief Skip next instruction if Vx != kk.
 *
 * The interpreter compares register Vx to kk, and if they are not equal,
 * increments the program counter by 2.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_4xkk(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t kk = self->op_code & 0x00FF;

  if (self->registers[Vx] != kk) {
    self->pc += 2;
  }

  printf("Checking if Vx != kk, 0x%04x 0x%04x, skip next instruction if true.\n", self->registers[Vx], kk);
}

/**
 * @breif Skip next instruction if Vx = Vy.
 *
 * The interpreter compares register Vx to register Vy, and if they are equal,
 * increments the program counter by 2.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_5xy0(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  if (self->registers[Vx] == self->registers[Vy]) {
    self->pc += 2;
  }

  printf("Checking if Vx = Vy, 0x%04x 0x%04x, skip next instruction if true.\n", self->registers[Vx], self->registers[Vy]);
}

/**
 * @brief Set Vx = kk.
 *
 * The interpreter puts the value kk into register Vx.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_6xkk(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t kk = self->op_code & 0x00FF;

  self->registers[Vx] = kk;

  printf("Setting register Vx to value 0x%04x\n", kk);
}

/**
 * @brief Set Vx = Vx + kk.
 *
 * Adds the value kk to the value of register Vx, then stores the result in Vx.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_7xkk(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t kk = self->op_code & 0x00FF;

  self->registers[Vx] = self->registers[Vx] + kk;

  printf("Register Vx 0x%04x + Value kk 0x%04x = 0x%04x.\n", self->registers[Vx] - kk, kk, self->registers[Vx]);
}

/**
 * @brief Skip next instruction if Vx != Vy.
 *
 * The values of Vx and Vy are compared, and if they are not equal, the program
 * counter is increased by 2.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_9xy0(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  if (self->registers[Vx] != self->registers[Vy]) {
    self->pc += 2;
  }

  printf("Checking if register Vx 0x%04x != register Vy 0x%04x, skip the next insturction if true.\n", self->registers[Vx], self->registers[Vy]);
}

/**
 * @brief Set I = nnn.
 *
 * The value of register I is set to nnn.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_annn(Chip8 *self) {
  self->ir = self->op_code & 0x0FFF;

  printf("Setting the Index register to 0x%04x.\n", self->ir);
}

/**
 * @brief Jump to location nnn + V0.
 *
 * The program counter is set to nnn plus the value of V0.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_bnnn(Chip8 *self) {
  uint16_t tmp = self->op_code & 0x0FFF;

  self->pc = tmp + self->registers[V0];

  printf("Jumping to location 0x%04x.\n", self->pc);
}

/**
 * @brief Set Vx = random byte AND kk.
 *
 * The interpreter generates a random number from 0 to 255, which is then ANDed
 * with the value kk. The results are stored in Vx. The function uses the raylib
 * library function GetRandomValue() to generate the random value.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_cxkk(Chip8 *self) {
  uint8_t tmp = self->op_code & 0x00FF;
  uint8_t random_byte = (uint8_t)GetRandomValue(0, 255);

  uint8_t Vx = GET_NIBBLE(self->op_code, 2);

  self->registers[Vx] = random_byte & tmp;

  printf("Setting the register Vx 0x%04x = 0x%04x & 0x%04x.\n", self->registers[Vx], random_byte, tmp);
}

/**
 * @brief  Display n-byte sprite starting at memory location I at (Vx, Vy), set
 * VF = collision.
 *
 *  The interpreter reads n bytes from memory, starting at the address stored in
 * I. These bytes are then displayed as sprites on screen at coordinates (Vx,
 * Vy). Sprites are XORed onto the existing screen. If this causes any pixels to
 * be erased, VF is set to 1, otherwise it is set to 0. If the sprite is
 * positioned so part of it is outside the coordinates of the display, it wraps
 * around to the opposite side of the screen. See instruction 8xy3 for more
 * information on XOR, and section 2.4, Display, for more information on the
 * Chip-8 screen and sprites.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_dxyn(Chip8 *self) {
  /* Number of bytes to be read from memory. */
  uint8_t num_of_bytes = GET_NIBBLE(self->op_code, 0);

  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  printf("The coordinates are (%d, %d)\n", self->registers[Vx], self->registers[Vy]);

  uint8_t sprite = 0;

  uint8_t i;

  for (i = 0; i <= num_of_bytes; i++) {
    sprite = self->memory[self->ir + i];
    printf("The value of the sprite is 0x%04x.\n", sprite);
    for (int j = 0; j < 8; j++) {
      if ((sprite & (0x80 >> j)) != 0) {
        if (self->graphics[(self->registers[Vx] + i) * CHIP8_SCREEN_WIDTH + (self->registers[Vy] + j)] != 0) {
          printf("The current graphics pixel at 0x%04x is not zero.\n", (self->registers[Vx] + i) * CHIP8_SCREEN_WIDTH + (self->registers[Vy] + j));
          self->registers[VF] = 1;
          self->graphics[(self->registers[Vx] + i) * CHIP8_SCREEN_WIDTH + (self->registers[Vy] + j)] ^= 1;
        }
      } else {
        printf("The current graphics pixel at 0x%04x is zero.\n", (self->registers[Vx] + i)  * CHIP8_SCREEN_WIDTH + ((self->registers[Vy] + j)));
        self->graphics[(self->registers[Vx] + i) * CHIP8_SCREEN_WIDTH + (self->registers[Vy] + j)] = 1;
      }
    }
  }

  int x = 0;
  printf("Dumping the graphics:\n");
  for (int i = 0; i < CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH; i++) {
    printf("0x%04x\t", self->graphics[i]);
    x++;
    if (x == 8) {
      printf("\n");
      x = 0;
    }
  }
  printf("\n");

  for (uint32_t i = 0; i < CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH; i++) {
    uint8_t y_cord = i % CHIP8_SCREEN_WIDTH;
    uint8_t x_cord = (i - y_cord) / CHIP8_SCREEN_WIDTH;
    if (self->graphics[i] == 1) {
      DrawRectangle(x_cord, y_cord, 20, 20, BLACK);
    }
  }



  /* for (int i = 0; i < CHIP8_SCREEN_HEIGHT; i++) { */
  /*   for (int j = 0; j < CHIP8_SCREEN_WIDTH; j++) { */
  /*     if ((self->graphics[j])  == 1) { */
  /*       printf("The current pixel %d, is %x.\n", j, self->graphics[i * CHIP8_SCREEN_WIDTH + j]); */
  /*       DrawRectangle(self->registers[Vx] + j, self->registers[Vy] + i, 1 * 20 , 1 * 20 , BLACK); */
  /*     } */
  /*   } */
  /* } */

  printf("Drawing.\n");
}

/**
 * @brief Set Vx = Vy
 * Stores the value of register Vy in register Vx.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xy0(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  self->registers[Vx] = self->registers[Vy];

  printf("Setting the register Vx 0x%04x to be the same as register Vy 0x%04x\n.", self->registers[Vx], self->registers[Vy]);
}

/**
 * @brief Set Vx = Vx OR Vy
 *
 * Performs a bitwise OR on the values of Vx and Vy, the stores the
 * result in Vx.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xy1(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  uint8_t tmp = self->registers[Vx];

  self->registers[Vx] = self->registers[Vx] | self->registers[Vy];

  printf("Setting the register Vx 0x%04x to be Vx 0x%04x | Vy 0x%04x\n.", self->registers[Vx], tmp, self->registers[Vy]);
}

/**
 * @brief Set Vx = Vx AND Vy
 *
 * Performs a bitwise AND on the values of Vx and Vy, then stores the
 * result in Vx.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xy2(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  uint8_t tmp = self->registers[Vx];

  self->registers[Vx] = self->registers[Vx] & self->registers[Vy];

  printf("Setting the register Vx 0x%04x to be Vx 0x%04x & Vy 0x%04x.\n", self->registers[Vx], tmp, self->registers[Vy]);
}

/**
 * @brief Set Vx = Vx XOR Vy.
 *
 * Performs a bitwise exclusive OR on the values of Vx and Vy, then
 * stores the result in Vx.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xy3(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  self->registers[Vx] = self->registers[Vx] ^ self->registers[Vy];

  printf("XORing register Vx, with register Vy\n.");
}

/**
 * @brief Set Vx = Vx + Vy, set VF = carry.
 *
 * The values of Vx and Vy are added together. If the result is greater than 8
 * bits (i.e., > 255,) VF is set to 1, otherwise 0.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xy4(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  uint16_t tmp = self->registers[Vx] + self->registers[Vy];

  self->registers[Vx] = self->registers[Vx] + self->registers[Vy];

  if (tmp > 255) {
    self->registers[VF] = 1;
  } else {
    self->registers[VF] = 0;
  }

  printf("Setting Vx += Vy.\n");
}

/**
 * @brief Set Vx = Vx - Vy, set VF = NOT borrow.
 *
 *  If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx,
 * and the results stored in Vx.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xy5(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  if (self->registers[Vx] > self->registers[Vy]) {
    self->registers[VF] = 1;
  } else {
    self->registers[VF] = 0;
  }

  self->registers[Vx] = self->registers[Vx] - self->registers[Vy];

  printf("Setting Vx -= Vy.\n");
}

/**
 * @brief Set Vx = Vx SHR 1.
 *
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
 * Then Vx is divided by 2.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xy6(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);

  if ((self->registers[Vx] & 0x1) == 1) {
    self->registers[VF] = 1;
  } else {
    self->registers[VF] = 0;
  }

  self->registers[Vx] /= 2;

  printf("Setting Vx SHR 1.\n");
}

/**
 * @brief  Set Vx = Vy - Vx, set VF = NOT borrow.
 *
 * If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy,
 * and the results stored in Vx.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xy7(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t Vy = GET_NIBBLE(self->op_code, 1);

  if (self->registers[Vy] > self->registers[Vx]) {
    self->registers[VF] = 1;
  } else {
    self->registers[VF] = 0;
  }

  self->registers[Vx] = self->registers[Vy] - self->registers[Vx];

  printf("Subtracting Vx from Vy.\n");
}

/**
 * @brief  Set Vx = Vx SHL 1.
 *
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
 * Then Vx is multiplied by 2.
 *
 * @param A pointer to the Chip8 object.
 */
void Chip8_OP_8xye(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);

  if (((self->registers[Vx] & 0xf) >> 3) == 1) {
    self->registers[VF] = 1;
  } else {
    self->registers[VF] = 0;
  }

  self->registers[Vx] *= 2;

  printf("Multiplying Vx * 2.\n");
}

/**
 * @brief Calls the specified 8xxx intruction.
 *
 * The function filters the 8xxx opcode by extracting the low nibble and calls
 * the speicified intruction accordingly.
 *
 * @param self A pointer to the Chip8 object.
 *
 */
void Chip8_extract_and_call_8xxx(Chip8 *self) {
  uint8_t low_nibble = GET_NIBBLE(self->op_code, 0);

  switch (low_nibble) {
  case 0x0:
    Chip8_OP_8xy0(self);
    break;
  case 0x1:
    Chip8_OP_8xy1(self);
    break;
  case 0x2:
    Chip8_OP_8xy2(self);
    break;
  case 0x3:
    Chip8_OP_8xy3(self);
    break;
  case 0x4:
    Chip8_OP_8xy4(self);
    break;
  case 0x5:
    Chip8_OP_8xy5(self);
    break;
  case 0x6:
    Chip8_OP_8xy6(self);
    break;
  case 0x7:
    Chip8_OP_8xy7(self);
    break;
  case 0xE:
    Chip8_OP_8xye(self);
    break;
  }
}

/**
 * @brief Skip next instruction if key with the value of Vx is not pressed.
 *
 * Checks the keyboard, and if the key corresponding to the value of Vx is
 * currently in the up position, PC is increased by 2.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_exa1(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);

  if (self->keypad[Vx] != 1) {
    self->pc += 2;
  }
}

/**
 * @brief  Skip next instruction if key with the value of Vx is pressed.
 *
 * Checks the keyboard, and if the key corresponding to the value of Vx is
 * currently in the down position, PC is increased by 2.
 *
 * @param self A pointer to to the Chip8 object.
 */
void Chip8_OP_ex9e(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);

  if (self->registers[Vx] == 1) {
    self->pc += 2;
  }
}

/**
 * @brief Calls the specified exxx intruction.
 *
 * The function filters the exxx opcode by extracting the low nibble and calls
 * the speicified intruction accordingly.
 *
 * @param self A pointer to the Chip8 object.
 *
 */
void Chip8_extract_and_call_exxx(Chip8 *self) {
  uint8_t low_nibble = GET_NIBBLE(self->op_code, 0);

  switch (low_nibble) {
  case 0x1:
    Chip8_OP_exa1(self);
    break;
  case 0xe:
    Chip8_OP_ex9e(self);
    break;
  }
}

/**
 * @brief Set Vx = delay timer value.
 *
 * The value of DT is placed into Vx.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_fx07(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);

  self->registers[Vx] = self->delay_timer;
}

/**
 * @brief Wait for a key press, store the value of the key in Vx.
 *
 * All execution stops until a key is pressed, then the value of that key is
 * stored in Vx.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_fx0a(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  self->registers[Vx] = -1;

  uint8_t i;
  for (i = 0; i <= 15; i++) {
    if (self->keypad[i] == 1) {
      self->registers[Vx] = i;
      break;
    }
  }

  if (self->registers[Vx] == -1) {
    self->pc -= 2;
  }
}

/**
 * @brief Set delay timer = Vx.
 *
 * DT is set equal to the value of Vx.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_fx15(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  self->delay_timer = self->registers[Vx];
}

/**
 * @brief  Set sound timer = Vx.
 *
 * Sound Timer is set equal to the value of Vx.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_fx18(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  self->sound_timer = self->registers[Vx];
}

/**
 * @brief Set I = I + Vx.
 *
 *  The values of I and Vx are added, and the results are stored in I.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_fx1e(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  self->ir += self->registers[Vx];
}

/**
 * @brief Set I = location of sprite for digit Vx.
 *
 * The value of I is set to the location for the hexadecimal sprite
 * corresponding to the value of Vx.
 *
 * @param self A pointer to the Chip8 object.
 *
 * TODO Make sure that this function operates correctly.
 */
void Chip8_OP_fx29(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  self->ir = FONTSET_START_ADDRESS +  self->registers[Vx];

  printf("Setting the IR to 0x%04x as the value of the Vx is 0x%04x.\n", self->ir, self->registers[Vx]);
}

/**
 * @brief Store BCD representation of Vx in memory locations I, I+1, and I+2.
 *
 * The interpreter takes the decimal value of Vx, and places the hundreds digit
 * in memory at location in I, the tens digit at location I+1, and the ones
 * digit at location I+2.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_fx33(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t tmp_value = self->registers[Vx];

  /* Ones places */
  self->memory[self->ir + 2] = tmp_value % 10;
  tmp_value /= 10;

  /* Tens place */
  self->memory[self->ir + 1] = tmp_value % 10;
  tmp_value /= 10;

  /* Hundreds place */
  self->memory[self->ir] = tmp_value % 10;
}

/**
 * @brief Store registers V0 through Vx in memory starting at location I.
 *
 * The interpreter copies the values of registers V0 through Vx into memory,
 * starting at the address in I.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_fx55(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t i = 0;

  for (i = 0; i <= Vx; i++) {
    self->memory[self->ir + i] = self->registers[i];
  }
}

/**
 * @brief Read registers V0 through Vx from memory starting at location I.
 *
 * The interpreter reads values from memory starting at location I into
 * registers V0 through Vx.
 *
 * @param self A pointer to the Chip8 object.
 */
void Chip8_OP_fx65(Chip8 *self) {
  uint8_t Vx = GET_NIBBLE(self->op_code, 2);
  uint8_t i;

  for (i = 0; i <= Vx; i++) {
    self->registers[i] = self->memory[self->ir + i];
  }
}

void Chip8_extract_and_call_fxxx(Chip8 *self) {
  uint8_t low_byte = GET_NIBBLE(self->op_code, 1) << 4;
  low_byte |= GET_NIBBLE(self->op_code, 0);

  switch (low_byte) {
  case 0x07:
    printf("Calling function fx07.\n");
    Chip8_OP_fx07(self);
    break;
  case 0x0A:
    printf("Calling function fx0A.\n");
    Chip8_OP_fx0a(self);
    break;
  case 0x15:
    printf("Calling function fx15.\n");
    Chip8_OP_fx15(self);
    break;
  case 0x18:
    printf("Calling function fx18.\n");
    Chip8_OP_fx18(self);
    break;
  case 0x1e:
    printf("Calling function fx1e.\n");
    Chip8_OP_fx1e(self);
    break;
  case 0x29:
    printf("Calling function fx29.\n");
    Chip8_OP_fx29(self);
    break;
  case 0x33:
    printf("Calling function fx33.\n");
    Chip8_OP_fx33(self);
    break;
  case 0x55:
    printf("Calling function fx55.\n");
    Chip8_OP_fx55(self);
    break;
  case 0x65:
    printf("Calling function fx65.\n");
    Chip8_OP_fx65(self);
    break;
  }
}

function chip8_instructions[16] = {Chip8_extract_and_call_0xxx,
                                   Chip8_OP_1nnn,
                                   Chip8_OP_2nnn,
                                   Chip8_OP_3xkk,
                                   Chip8_OP_4xkk,
                                   Chip8_OP_5xy0,
                                   Chip8_OP_6xkk,
                                   Chip8_OP_7xkk,
                                   Chip8_extract_and_call_8xxx,
                                   Chip8_OP_9xy0,
                                   Chip8_OP_annn,
                                   Chip8_OP_bnnn,
                                   Chip8_OP_cxkk,
                                   Chip8_OP_dxyn,
                                   Chip8_extract_and_call_exxx,
                                   Chip8_extract_and_call_fxxx};

void chip8_inst_emulate(Chip8 *self) {
  chip8_instructions[GET_NIBBLE(self->op_code, 3)](self);
}
