# Compiler and linker settings
CC = gcc
CFLAGS = -Wall -Ichip8/include -Igame/include -Iraylib/include -g
LDFLAGS = -Lraylib/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Directories
SRC_DIR := chip8/src
OBJ_DIR := chip8/obj
RAYLIB_DIR = raylib
BUILD_DIR = build

# Source files and object files
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))
MAIN_SRC = main.c

# Executable name
EXECUTABLE = output

# Targets
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ_FILES) $(MAIN_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJ_DIR)

.PHONY: all clean
