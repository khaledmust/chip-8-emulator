#include "chip8.h"
#include "game.h"
#include "raylib.h"
#include <stdint.h>
#include <stdio.h>

#define SPECIFIED_TIME 0.0166

void keyboard_control(Chip8 *self) {
  for (int i = 0; i < 16; i++) {
    printf("The state of the keyboard is %d.\n", self->keypad[i]);
  }

  if (IsKeyDown(KEY_X)) {
    self->keypad[0] = 1;
  } else {
    self->keypad[0] = 0;
  }

  if (IsKeyDown(KEY_ONE)) {
    self->keypad[1] = 1;
  } else {
    self->keypad[1] = 0;
  }

  if (IsKeyDown(KEY_TWO)) {
    self->keypad[2] = 1;
  } else {
    self->keypad[2] = 0;
  }

  if (IsKeyDown(KEY_THREE)) {
    self->keypad[3] = 1;
  } else {
    self->keypad[3] = 0;
  }

  if (IsKeyDown(KEY_Q)) {
    self->keypad[4] = 1;
  } else {
    self->keypad[4] = 0;
  }

  if (IsKeyDown(KEY_W)) {
    self->keypad[5] = 1;
  } else {
    self->keypad[5] = 0;
  }

  if (IsKeyDown(KEY_E)) {
    self->keypad[6] = 1;
  } else {
    self->keypad[6] = 0;
  }

  if (IsKeyDown(KEY_A)) {
    self->keypad[7] = 1;
  } else {
    self->keypad[7] = 0;
  }

  if (IsKeyDown(KEY_S)) {
    self->keypad[8] = 1;
  } else {
    self->keypad[8] = 0;
  }

  if (IsKeyDown(KEY_D)) {
    self->keypad[9] = 1;
  } else {
    self->keypad[9] = 0;
  }

  if (IsKeyDown(KEY_Z)) {
    self->keypad[10] = 1;
  } else {
    self->keypad[10] = 0;
  }

  if (IsKeyDown(KEY_C)) {
    self->keypad[11] = 1;
  } else {
    self->keypad[11] = 0;
  }

  if (IsKeyDown(KEY_FOUR)) {
    self->keypad[12] = 1;
  } else {
    self->keypad[12] = 0;
  }

  if (IsKeyDown(KEY_R)) {
    self->keypad[13] = 1;
  } else {
    self->keypad[13] = 0;
  }

  if (IsKeyDown(KEY_F)) {
    self->keypad[14] = 1;
  } else {
    self->keypad[14] = 0;
  }

  if (IsKeyDown(KEY_V)) {
    self->keypad[15] = 1;
  } else {
    self->keypad[15] = 0;
  }
}

int main(int argc, char **argv) {
  // Initialization
  InitWindow(screenWidth, screenHeight, "Chip-8 Emulator");

  SetTargetFPS(60); // Set our game to run at 60 frames-per-second

  Chip8 myChip;
  chip8_init(&myChip);
  chip8_load_rom(&myChip, argv[1]);
  Rectangle pixel = {.height = 10, .width = 10};
  // chip8_parse_code(&myChip);
  double current_time;
  double last_update_time;

  // Main game loop
  while (!WindowShouldClose()) {

    keyboard_control(&myChip);

    current_time = GetTime();
    printf("The current time: %lf\n", current_time);
    if (current_time - last_update_time >= SPECIFIED_TIME) {
      if (myChip.delay_timer > 0) {
        myChip.delay_timer--;
      }
      last_update_time = current_time;
    }

    // Draw
    BeginDrawing();
    ClearBackground(GREEN);

    chip8_parse_code(&myChip);
    chip8_inst_emulate(&myChip);

    printf("==========Dumping registers==========\n");
    printf("V0: 0x%02x\t, V1: 0x%02x\n", myChip.registers[0],
           myChip.registers[1]);
    printf("V2: 0x%02x\t, V3: 0x%02x\n", myChip.registers[2],
           myChip.registers[3]);
    printf("V4: 0x%02x\t, V5: 0x%02x\n", myChip.registers[4],
           myChip.registers[5]);
    printf("V6: 0x%02x\t, V7: 0x%02x\n", myChip.registers[6],
           myChip.registers[7]);
    printf("V8: 0x%02x\t, V9: 0x%02x\n", myChip.registers[8],
           myChip.registers[9]);
    printf("VA: 0x%02x\t, VB: 0x%02x\n", myChip.registers[10],
           myChip.registers[11]);
    printf("VC: 0x%02x\t, VD: 0x%02x\n", myChip.registers[12],
           myChip.registers[13]);
    printf("VE: 0x%02x\t, VF: 0x%02x\n", myChip.registers[14],
           myChip.registers[15]);
    printf("Index Registers: 0x%04x\n", myChip.ir);
    printf("Program counter: 0x%04x\n", myChip.pc);
    printf("The content of the stack 0: 0x%04x\n", myChip.stack[0]);
    printf("The content of the stack 1: 0x%04x\n", myChip.stack[1]);
    printf("The content of the stack 2: 0x%04x\n", myChip.stack[2]);
    printf("================End=================\n");

    for (int i = 0; i < CHIP8_SCREEN_HEIGHT; i++) {
      for (int j = 0; j < CHIP8_SCREEN_WIDTH; j++) {
        if (myChip.graphics[j][i] == 1) {
          pixel.x = j * 10;
          pixel.y = i * 10;
          DrawRectangleRounded(pixel, 0.1, 6, BLACK);
        }
      }
    }

    EndDrawing();
  }

  // De-Initialization
  CloseWindow();

  return 0;
}
