CC := clang
CFLAGS := -Wall -Wextra -I. -Isrc/include -g
ASAN_FLAGS := -fsanitize=address -fno-omit-frame-pointer

RAYLIB_INC := /opt/homebrew/include
RAYLIB_LIB := /opt/homebrew/lib

CFLAGS += -I$(RAYLIB_INC)

LDFLAGS := -L$(RAYLIB_LIB) -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreAudio -framework CoreVideo -lm

ifeq ($(ASAN),1)
  CFLAGS += $(ASAN_FLAGS)
endif

SRC := $(shell find src -name '*.c')
OBJ := $(patsubst src/%.c,bin/%.o,$(SRC))
EXEC := bin/main

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ) | bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin:
	mkdir -p bin

bin/%.o: src/%.c | bin
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin
