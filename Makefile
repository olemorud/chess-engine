
BUILD ?= debug
CC := clang

CFLAGS.gcc := -std=c23 -Wall -Wextra -Wconversion -Wno-unused-function
CFLAGS.gcc.release := -Ofast -march=native -DNDEBUG
CFLAGS.gcc.debug := -ggdb -O0 -fsanitize=address

CFLAGS.clang := -std=c23 -g -Wall -Wextra -Wconversion -Wno-unused-function -Wimplicit-int-conversion
CFLAGS.clang.release := -O3 -ffast-math -march=native # -DNDEBUG
CFLAGS.clang.debug := -g3 -O0 -fsanitize=address
CFLAGS.clang.wasm := \
	--target=wasm32-unknown-unknown -nostdlib -g \
	-Wl,--export-all \
	-Wl,--no-entry

CFLAGS :=  $(CFLAGS.$(CC)) $(CFLAGS.$(CC).$(BUILD))

all: tests

wasm: chess.wasm

codegen: codegen.c
	$(CC) -o $@ $(CFLAGS) $^

chess.wasm: wasm-compat-chatgpt.c mbb_rook.h mbb_bishop.h engine.h
	$(CC) -DWASM -o $@ wasm-compat-chatgpt.c $(CFLAGS.$(CC)) $(CFLAGS.$(CC).wasm)

mbb_rook.h: codegen
	./codegen

mbb_bishop.h: codegen
	./codegen

tests: tests.c mbb_rook.h mbb_bishop.h engine.h
	$(CC) -o $@ $(CFLAGS) -DUSE_PRINTF tests.c
