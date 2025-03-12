# Compiler
CC = clang

# Compiler flags
CFLAGS = -Wall -Wextra -Werror

# Libraries
LIBS = -lncurses

# Source and output
SRC = game_logic_test.c
OUT = game_logic_test

# Default target
all: $(OUT)

# Build the executable
$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $(OUT) $(SRC) $(LIBS)

# Clean target
clean:
	rm -f $(OUT)

# Phony targets
.PHONY: all clean
