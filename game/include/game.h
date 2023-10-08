#ifndef GAME_H_
#define GAME_H_

#include "chip8.h"

#define OFFSET 200

#define screenWidth CHIP8_SCREEN_WIDTH * 10
#define screenHeight CHIP8_SCREEN_HEIGHT * 10

typedef struct{
    Chip8 chip8;
}Game;

#endif // GAME_H_
