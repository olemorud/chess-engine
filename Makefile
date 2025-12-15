
BUILD ?= debug
CC := clang

CFLAGS.gcc.release := -O3
CFLAGS.gcc.debug := -ggdb -O0 -fsanitize=address
CFLAGS.gcc := -std=c23 -Wall -Wextra -Wconversion -Wno-unused-function

CFLAGS.clang.release := -O3
CFLAGS.clang.debug := -ggdb -O0
CFLAGS.clang := -std=c23 -Wall -Wextra -Wconversion -Wno-unused-function -Wimplicit-int-conversion

CFLAGS :=  $(CFLAGS.$(CC)) $(CFLAGS.$(CC).$(BUILD))

all: tests

codegen: codegen.c
	$(CC) -o $@ $(CFLAGS) $^

mbb_rook.h: codegen
	./codegen

mbb_bishop.h: codegen
	./codegen

tests: tests.c mbb_rook.h mbb_bishop.h base.h
	$(CC) -o $@ $(CFLAGS) tests.c
