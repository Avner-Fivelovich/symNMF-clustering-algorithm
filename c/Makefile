# Compiler and flags
CC = gcc
CFLAGS = -ansi -Wall -Wextra -Werror -pedantic-errors

# Source files for C executable
CSRC = symnmf.c
COBJ = $(CSRC:.c=.o)

# Python source files
PYSRC = symnmfmodule.c setup.py symnmf.py

# Header files
DEPS = symnmf.h

# Output executable
EXEC = symnmf

# Build Python module
PYTHON = python

# Default target
all: $(EXEC) $(PYTHON)

# Build the executable
$(EXEC): $(COBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(COBJ) -lm

# Build object files
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

# Build Python module
$(PYTHON): $(COBJ) $(PYSRC)
	python3 setup.py build_ext --inplace

# Clean target to remove built files
clean:
	rm -f $(COBJ) $(EXEC) symnmflib.cpython-36m-x86_64-linux-gnu.so
	rm -rf build __pycache__

# Phony targets
.PHONY: python all clean

