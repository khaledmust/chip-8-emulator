#include <stdint.h>
#include <stdio.h>
#include "chip8.h"
#include "game.h"
#include "raylib.h"

int main(int argc, char **argv)
{
    // Initialization

    InitWindow(screenWidth, screenHeight, "Chip-8 Emulator by Khaled Mustafa");

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second

    Chip8 myChip;
    chip8_init(&myChip);
    chip8_load_rom(&myChip, argv[1]);
    //chip8_parse_code(&myChip);

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Update your variables here
        //----------------------------------------------------------------------------------

        // Draw
        chip8_parse_code(&myChip);
        BeginDrawing();
        ClearBackground(GREEN);
        chip8_inst_emulate(&myChip);

        for (uint16_t i = 0; i < CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH; i++) {
            if (myChip.graphics[i] == 1) {
                DrawRectangle((uint8_t)(i % CHIP8_SCREEN_WIDTH), (uint8_t)(i / CHIP8_SCREEN_WIDTH), 1, 1, BLACK);
                printf("The coordinates are %d, %d.\n", i / CHIP8_SCREEN_WIDTH, i % CHIP8_SCREEN_WIDTH);
            }
        }

        /* for (int i = 0; i < CHIP8_SCREEN_HEIGHT; i++) { */
        /*     for (int j = 0; j < CHIP8_SCREEN_WIDTH; j++) { */
        /*         if (myChip.graphics[k] == 1) { */
        /*             DrawRectangle(j, i, 1, 1, BLACK); */
        /*                 } */
        /*         k++; */
        /*     } */
        /* } */

        /* for (uint32_t i = 0; i < CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH; */
        /*      i++) { */
        /*     if (myChip.graphics[i] == 1) { */
        /*         int x = i / 64; */
        /*         int y = i % 64; */
        /*         DrawRectangle(x, y, 1, 1, BLACK); */
        /*     } */
        /* } */

        // DrawText("Congrats! You created your first window!", 190, 200, 20,
        // LIGHTGRAY);

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();

    return 0;
}
