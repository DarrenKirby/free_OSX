# Makefile for free program

# Default compiler choice
CC ?= gcc

# Use clang if gcc is missing
ifeq (, $(shell which gcc))
    CC := clang
endif

# Compiler flags
CFLAGS := -Wall -Wextra -Werror -pedantic -O2

# Target binary
TARGET := free

# Source files
SRC := free.c

# Build target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Install target
install: $(TARGET)
	@echo "Installing the binary to /usr/local/bin"
	@sudo cp $(TARGET) /usr/local/bin/
	@echo "Installing the manpage to /usr/local/man/man1"
	@sudo cp free.1 /usr/local/man/man1/
	@echo "Installation complete."

# Clean target
clean:
	rm -f $(TARGET)

.PHONY: install clean
