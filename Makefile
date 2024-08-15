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

# local/man does not exist by default on OS X
MANDIR = /usr/local/man/man1/
INSDIR = /usr/local/bin/
MANPAGE = free.1

# Build target
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Install target
install: $(TARGET)
	@echo "Installing the binary to $(INSDIR)"
	@sudo cp $(TARGET) $(INSDIR)
	@echo "Installing the manpage to $(MANDIR)"
	[ -d $(MANDIR) ] || mkdir -p $(MANDIR)
	@sudo cp $(MANPAGE) $(MANDIR)
	@echo "Installation complete."

uninstall:
	@sudo rm ${INSDIR}$(TARGET)
	@sudo rm $(MANDIR)$(MANPAGE)
	@echo "Uninstallation complete"

# Clean target
clean:
	rm -f $(TARGET)

.PHONY: install clean
