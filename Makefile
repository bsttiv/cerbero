CC := gcc
CFLAGS  := -Wall -Wextra -pedantic -std=c11 -g -I./include

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include

SRCS    := $(wildcard $(SRC_DIR)/*.c)
OBJS    := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TARGET  := cerbero

.PHONY: all clean run memcheck

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: all
	./$(TARGET)

memcheck: all
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)
