
# How the compilation works:
# Every .c file is compiled into its own .o file.  Methods will be compiled
# into .o files inside methods/bin/ and other .c files such as main.c will be
# compiled into bin/.  Then the executables are created by linking main.o with
# the method .o file.  Each executable is created in bin/<method_name>.
#
# All machine specific settings are in settings.mk, which is coppied from
# settings.mk.default

SHELL = /bin/sh
CC=gcc

# Handle settings.mk
$(shell cp -u settings.mk.default settings.mk)
include settings.mk

LEVMAR_CC_FLAGS=-I$(LEVMAR_DIRECTORY)
LEVMAR_LD_FLAGS=$(LEVMAR_FLAGS) \
    $(LEVMAR_DIRECTORY)/Axb.o \
    $(LEVMAR_DIRECTORY)/lmbc.o \
    $(LEVMAR_DIRECTORY)/lmblec.o \
    $(LEVMAR_DIRECTORY)/lmbleic.o \
    $(LEVMAR_DIRECTORY)/lmlec.o \
    $(LEVMAR_DIRECTORY)/lm.o \
    $(LEVMAR_DIRECTORY)/misc.o
CFLAGS=-g --std=c99 -pedantic -Wall -Werror -Wno-unused -I./ $(LEVMAR_CC_FLAGS)
OPENCV_FLAGS=$(OPENCV_EXTRA_FLAGS) -lcxcore -lcv -lhighgui -lcvaux \
             -lml -DCV_NO_BACKWARD_COMPATIBILITY

# List all .c files in methods/ folder
METHOD_EXECUTABLES = $(patsubst methods/%.c, bin/%, $(wildcard methods/*.c))

all: bin/ methods/bin/ $(METHOD_EXECUTABLES)

# Method Executable
bin/%: methods/bin/%.o bin/main.o bin/color_tools.o
	$(CC) $(OPENCV_FLAGS) $(LEVMAR_LD_FLAGS) $(CFLAGS) $^ -o $@

# Objects
bin/%.o: %.c
	$(CC) $(OPENCV_FLAGS) $(CFLAGS) -c $*.c -o bin/$*.o

# Method Objects
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
