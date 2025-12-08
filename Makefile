
CFLAGS := -std=c23

all: tests

codegen: codegen.c
	$(CC) -o $@ $(CFLAGS) -DCODEGEN $^

mbb_rook.h: codegen
	./codegen

mbb_bishop.h: codegen
	./codegen

tests: tests.c | mbb_rook.h mbb_bishop.h
	$(CC) -o $@ $(CFLAGS) $^
