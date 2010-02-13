SHELL = /bin/sh
CC=gcc
CFLAGS=-g --std=c99 -pedantic -Wall -Werror -Wno-unused
OPENCV_FLAGS=-I/usr/local/include/opencv -lcxcore -lcv -lhighgui -lcvaux -lml \
             -DCV_NO_BACKWARD_COMPATIBILITY

METHOD_EXECUTABLES = $(patsubst methods/%.c, bin/%, $(wildcard methods/*.c))

all: bin/ methods/bin/ $(METHOD_EXECUTABLES)

# Method Executable
bin/%: methods/bin/%.o bin/main.o
	$(CC) $(OPENCV_FLAGS) $(CFLAGS) bin/main.o $< -o $@

# Main Object
bin/main.o: main.c
	$(CC) $(OPENCV_FLAGS) $(CFLAGS) -c main.c -o $@

# Method Object
methods/bin/%.o: methods/%.c
	$(CC) $(OPENCV_FLAGS) $(CFLAGS) -c methods/$*.c -o methods/bin/$*.o

# Directories
bin/:
	-mkdir bin
methods/bin/:
	-mkdir methods/bin

clean:
	-rm bin/*
	-rm methods/bin/*.o
