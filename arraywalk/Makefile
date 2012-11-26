# One could play with compiler optimizations to see whether those have any
# effect. Expected: not much. ;-)
CFLAGS=-std=c99 -D_POSIX_C_SOURCE=200809L -W -Wall -Wextra -pedantic -g -O0
# Link with liblrt for timing events.
LDFLAGS=-lrt

# Default target is main, rest is handled by GNU Make builtin rules.
all: main

main: main.c arraywalk.c util.c

# Some housekeeping does no harm.
clean:
	rm -f main

.PHONY: clean
