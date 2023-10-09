#include "chip8.h"
#include "game.h"
#include "raylib.h"
#include <stdint.h>
#include <stdio.h>

int main(int argc, char **argv) {
  // Initialization
  InitWindow(screenWidth, screenHeight, "Chip-8 Emulator");

  /* Set the game to run at 60 frames-per-second */
  SetTargetFPS(60);

  Chip8 myChip;

  /* Screen pixel parameters. */
  Rectangle pixel = {.height = 10, .width = 10};

  chip8_init(&myChip);
  chip8_load_rom(&myChip, argv[1]);

  // Main game loop
  while (!WindowShouldClose()) {

    chip8_timer_control(&myChip);

    chip8_keyboard_control(&myChip);

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

    chip8_draw(&myChip, &pixel);

    EndDrawing();
  }

  // De-Initialization
  CloseWindow();

  return 0;
}
