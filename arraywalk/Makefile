# One could play with compiler optimizations to see whether those have any
# effect. Expected: not much. ;-)
CFLAGS=-w -Wall -Wextra -pedantic -g -O0
# Link with liblrt for timing events.
LDFLAGS=-lrt

# Default target is main, rest is handled by GNU Make builtin rules.
all: main

# Some housekeeping does no harm.
clean:
	rm -f main

.PHONY: clean
