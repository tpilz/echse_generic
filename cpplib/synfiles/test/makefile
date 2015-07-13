# Name of the executable to be built
BIN= test

# Lists of the project-specific source and header files
CPP= ../synfiles.cpp test.c++
HDR= ../synfiles.h

# The top-level directory of the DKLIB code collection
# The 1st level sub-directories of this top-level directory contain ...
#   (1) precompiled object files (.o) to be used by the linker
#   (2) header files (.h) included by this project's source files
DKLIB= /home/dkneis/cpp/dklib

# List of object files to be linked against (precompiled DKLIB code)
OBJ= $(DKLIB)/except/except.o \
     $(DKLIB)/typeconv/typeconv.o \
     $(DKLIB)/stringmanip/stringmanip.o \
     $(DKLIB)/fixedZoneTime/fixedZoneTime.o  

# Compiler commands
CC     = /usr/bin/gcc
CFLAGS = -ansi -I$(DKLIB) -lm -lstdc++ -pedantic -Wall -Wextra -ftrapping-math -ffast-math -fbounds-check -O2

# The one and only target (default target)
all: $(OBJ) $(HDR) $(CPP)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJ) $(CPP)

