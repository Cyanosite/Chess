TARGET_EXEC := main

BUILD_DIR := ./build
SRC_DIR := ./src
CC := gcc -g

SRCS := $(shell find $(SRC_DIRS) -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

CFLAGS := `sdl2-config --cflags --libs` -lSDL2_gfx -lSDL2_ttf -lSDL2_image

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(CFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	 rm -r $(BUILD_DIR)/$(SRC_DIR)