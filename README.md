# chip-8-emulator

This Chip-8 emulator is implemented in C, leveraging the RayLib library for graphical and input handling capabilities.

## Table of Contents
- [Introduction](#introduction)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Technical Implementation](#technical-implementation)
- [Screenshots](#screenshots)
- [License](#license)

## Introduction

> Chip-8 is a simple, interpreted, programming language which was first used on some do-it-yourself computer systems in the late 1970s and early 1980s.

The Chip-8 Emulator project is my way of paying homage to early video games and learning more about how computers work. Chip-8 is an old and basic computer system from the late 1970s and early 1980s. It was used for simple games and programs on early computers.

I decided to create this emulator to have fun and to understand the basics of computer systems. Making the Chip-8 games run on modern computers helps us appreciate the history of gaming and how computers have evolved. During this project, I've learned about how computer instructions are translated, how memory works, and how pictures are drawn on the screen.

Along this project I have learned a lot including but not limited to:
- Writing and understanding a 8-bit and 16-bit hex dump
- How to disassemble and decode an opcode into instructions a CPU can use
- How a CPU can utilize memory, stack, program counters, stack pointers, memory addresses, and registers
- How a CPU implements fetch, decode, and execute

I hope my version of the Chip-8 emulator can easy to easily understood by beginners, as I have tried my outmost effort to write easy-to-understand code, and for this is the reason for me deciding to utilize RayLib, as it's an easy graphical library.

## Getting Started

To be able to build the emulator you will need to install [RayLib](https://www.raylib.com/).

### 1. Clone the repository:

``` sh
git clone https://github.com/khaledmust/chip-8-emulator.git
```

### 2. Navigate to the project directory

``` sh
cd chip-8-emulator
```

### 3. Compile and run the game by providing the ROM name and path

``` sh
make

./output ./roms/ROM_NAME
```

## Usage

The Chip-8 keypad has been mapped to the following keys:

``` text
Keypad                   Keyboard
+-+-+-+-+                +-+-+-+-+
|1|2|3|C|                |1|2|3|4|
+-+-+-+-+                +-+-+-+-+
|4|5|6|D|                |Q|W|E|R|
+-+-+-+-+       =>       +-+-+-+-+
|7|8|9|E|                |A|S|D|F|
+-+-+-+-+                +-+-+-+-+
|A|0|B|F|                |Z|X|C|V|
+-+-+-+-+                +-+-+-+-+
```

## Technical Implementation

### Chip-8 Memory Layout

![chip-8 memory layout](https://github.com/khaledmust/chip-8-emulator/blob/main/chip-8-memory-layout.png)

### Chip-8 Instructions

The Chip-8 has 36 different instructions, including math, graphics, and flow control functions. All the instructions are 2-bytes long and are stored in the most-significant-byte first.

All those instructions are implemented in the `chip8.c` file.

The emulator starts by calling `chip8_parse_code` function, this function first parses 8-bits of opcode and then increment the PC to point
to the next 8-bits opcode which is then`OR` to the previous one to form 16-bits instruction.

![chip-8 parsing opcode](https://github.com/khaledmust/chip-8-emulator/blob/main/chip-8-parsing-opcode.png)

Then the function `chip8_inst_emulate` is called which executes the opcode by first, calling an array of function pointer, then, executing the function the corresponds to the extracted nibble from the opcode.

![chip-8 instruction table](https://github.com/khaledmust/chip-8-emulator/blob/main/chip-8-instruction-table.png)

## Screenshots

![](https://github.com/khaledmust/chip-8-emulator/blob/main/screenshots/Screenshot%20from%202023-10-10%2012-16-07.png)

![](https://github.com/khaledmust/chip-8-emulator/blob/main/screenshots/Screenshot%20from%202023-10-10%2012-16-23.png)

![](https://github.com/khaledmust/chip-8-emulator/blob/main/screenshots/Screenshot%20from%202023-10-10%2012-16-35.png)

![](https://github.com/khaledmust/chip-8-emulator/blob/main/screenshots/Screenshot%20from%202023-10-10%2012-17-20.png)

![](https://github.com/khaledmust/chip-8-emulator/blob/main/screenshots/Screenshot%20from%202023-10-10%2012-18-46.png)

## License
This project is open source and available under the MIT License.

