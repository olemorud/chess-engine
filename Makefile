
BUILD ?= debug
CC := clang

CFLAGS.gcc := -std=c23 -Wall -Wextra -Wconversion -Wno-unused-function
CFLAGS.gcc.release := -Ofast
CFLAGS.gcc.debug := -ggdb -O0 -fsanitize=address

CFLAGS.clang := -std=c23 -Wall -Wextra -Wconversion -Wno-unused-function -Wimplicit-int-conversion
CFLAGS.clang.release := -Ofast -march=native
CFLAGS.clang.debug := -ggdb -O0 -fsanitize=address
CFLAGS.clang.wasm := \
	--target=wasm32-unknown-unknown -O3 -nostdlib \
	-Wl,--export-all \
	-Wl,--no-entry

CFLAGS :=  $(CFLAGS.$(CC)) $(CFLAGS.$(CC).$(BUILD))

all: tests

wasm: chess.wasm

codegen: codegen.c
	$(CC) -o $@ $(CFLAGS) $^

chess.wasm: wasm-compat.c
	$(CC) -DWASM -o $@ wasm-compat.c $(CFLAGS.$(CC)) $(CFLAGS.$(CC).wasm) 

mbb_rook.h: codegen
	./codegen

mbb_bishop.h: codegen
	./codegen

tests: tests.c mbb_rook.h mbb_bishop.h engine.h
	$(CC) -o $@ $(CFLAGS) tests.c
