#ifndef GAME_H_
#define GAME_H_

#include "chip8.h"

#define OFFSET 386

#define screenWidth CHIP8_SCREEN_WIDTH + OFFSET
#define screenHeight CHIP8_SCREEN_HEIGHT + OFFSET

typedef struct{
    Chip8 chip8;
}Game;

#endif // GAME_H_
