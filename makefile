
LEVMAR_DIRECTORY=/home/michael/install/levmar/
##### MODIFYABLE SETTINGS ABOVE THIS LINE #####



# How this works:
# Every .c file is compiled into its own .o file.  Methods will be compiled
# into .o files inside methods/bin/ and other .c files such as main.c will be
# compiled into bin/.  Then the executables are created by linking main.o with
# the method .o file.  Each executable is created in bin/<method_name>.

SHELL = /bin/sh
CC=gcc
LEVMAR_FLAGS=-I$(LEVMAR_DIRECTORY) -L$(LEVMAR_DIRECTORY) -L/usr/local/lib -llevmar -llapack -lblas -lf2c  -lm -Wl,-t -L./ -u MAIN__ \
    $(LEVMAR_DIRECTORY)/Axb.o \
    $(LEVMAR_DIRECTORY)/lmbc.o \
    $(LEVMAR_DIRECTORY)/lmblec.o \
    $(LEVMAR_DIRECTORY)/lmbleic.o \
    $(LEVMAR_DIRECTORY)/lmlec.o \
    $(LEVMAR_DIRECTORY)/lm.o \
    $(LEVMAR_DIRECTORY)/misc.o
CFLAGS=-g --std=c99 -pedantic -Wall -Werror -Wno-unused -I./ $(LEVMAR_FLAGS)
# TODO: This might should use pkg-config for compatibility, but many opencv
# installations won't have .pc files, such as MacPorts installations.  It is
# still undecided exactly what to do about this compatability, but for now,
# changing OPENCV_FLAGS manually will have to do.
OPENCV_FLAGS=-I/usr/local/include/opencv -lcxcore -lcv -lhighgui -lcvaux -lml \
             -DCV_NO_BACKWARD_COMPATIBILITY

METHOD_EXECUTABLES = $(patsubst methods/%.c, bin/%, $(wildcard methods/*.c))

all: bin/ methods/bin/ $(METHOD_EXECUTABLES)

# Method Executable
bin/%: methods/bin/%.o bin/main.o bin/color_tools.o
	$(CC) $(OPENCV_FLAGS) $(CFLAGS) $^ -o $@

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
