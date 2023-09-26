# Compiler and linker settings
CC = gcc
CFLAGS = -Wall -Ichip8/include -Igame/include -Iraylib/include -g
LDFLAGS = -Lraylib/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
VALGRIND = valgrind
VALGRINDFLAGS = --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt

# Directories
CHIP8_SRC_DIR := chip8/src
CHIP8_OBJ_DIR := chip8/obj
GAME_SRC_DIR := game/src
GAME_OBJ_DIR := game/obj
RAYLIB_DIR = raylib
BUILD_DIR = build

# Source files and object files
CHIP8_SRC_FILES := $(wildcard $(CHIP8_SRC_DIR)/*.c)
CHIP8_OBJ_FILES := $(patsubst $(CHIP8_SRC_DIR)/%.c, $(CHIP8_OBJ_DIR)/%.o, $(CHIP8_SRC_FILES))
GAME_SRC_FILES := $(wildcard $(GAME_SRC_DIR)/*.c)
GAME_OBJ_FILES := $(patsubst $(GAME_SRC_DIR)/%.c, $(GAME_OBJ_DIR)/%.o, $(GAME_SRC_FILES))
MAIN_SRC = main.c

# Executable name
EXECUTABLE = output

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(CHIP8_OBJ_FILES) $(GAME_OBJ_FILES) $(MAIN_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(CHIP8_OBJ_DIR)/%.o: $(CHIP8_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(GAME_OBJ_DIR)/%.o: $(GAME_SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(CHIP8_OBJ_DIR) $(GAME_OBJ_DIR)

valgrind:
	$(VALGRIND) $(VALGRINDFLAGS) ./$(EXECUTABLE)

.PHONY: all clean valgrind
