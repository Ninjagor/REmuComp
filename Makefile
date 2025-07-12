CC := clang
CFLAGS := -Wall -Wextra -I. -Isrc/include -g
ASAN_FLAGS := -fsanitize=address -fno-omit-frame-pointer

ifeq ($(ASAN),1)
  CFLAGS += $(ASAN_FLAGS)
endif

SRC := $(shell find src -name '*.c')
OBJ := $(patsubst src/%.c,bin/%.o,$(SRC))
EXEC := bin/main

.PHONY: all clean

all: $(EXEC)

$(EXEC): $(OBJ) | bin
	$(CC) $(CFLAGS) -o $@ $^

bin:
	mkdir -p bin

bin/%.o: src/%.c | bin
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf bin
