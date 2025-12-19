
BUILD ?= debug
CC := clang

CFLAGS.gcc := -std=c23 -Wall -Wextra -Wconversion -Wno-unused-function
CFLAGS.gcc.release := -Ofast
CFLAGS.gcc.debug := -ggdb -O0 -fsanitize=address

CFLAGS.clang := -std=c23 -Wall -Wextra -Wconversion -Wno-unused-function -Wimplicit-int-conversion
CFLAGS.clang.release := -Ofast
CFLAGS.clang.debug := -ggdb -O0 -fsanitize=address
CFLAGS.clang.wasm := \
	--target=wasm32-unknown-unknown -O3 -nostdlib \
	-Wl,--export-all \
	-Wl,--no-entry

CFLAGS :=  $(CFLAGS.$(CC)) $(CFLAGS.$(CC).$(BUILD))

all: tests

codegen: codegen.c
	$(CC) -o $@ $(CFLAGS) $^

wasm: wasm-compat.c
	$(CC) -DWASM -o chess.wasm wasm-compat.c $(CFLAGS.$(CC)) $(CFLAGS.$(CC).wasm) 

mbb_rook.h: codegen
	./codegen

mbb_bishop.h: codegen
	./codegen

tests: tests.c mbb_rook.h mbb_bishop.h base.h
	$(CC) -o $@ $(CFLAGS) tests.c
