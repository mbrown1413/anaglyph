SHELL = /bin/sh
CC=gcc
CFLAGS=-g --std=c99 -pedantic -Wall -Werror -Wno-unused
OPENCV_FLAGS=-I/usr/local/include/opencv -lcxcore -lcv -lhighgui -lcvaux -lml \
             -DCV_NO_BACKWARD_COMPATIBILITY

ALL_FLAGS = $(CFLAGS) $(OPENCV_FLAGS)

all: main

main: main.c
	$(CC) $(ALL_FLAGS) $@.c -o $@
