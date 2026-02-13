# TeaDry Controller - Makefile
# C99, gcc/clang

CC    ?= gcc
CFLAGS = -std=c99 -Wall -Wextra -Iinclude
LDFLAGS =
SRC   = src/main.c src/control.c src/log.c src/sensor.c src/state.c
OUT   = tea_dry

.PHONY: all build clean run help

all: build

help:
	@echo "TeaDry Controller - targets: build, clean, run"

build: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)

run: build
	./$(OUT) 300
