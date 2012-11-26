# One could play with compiler optimizations to see whether those have any
# effect.
CFLAGS=-std=c99 -D_POSIX_C_SOURCE=200809L -W -Wall -Wextra -pedantic -g -O3 -funroll-loops
#CFLAGS=-std=c99 -D_POSIX_C_SOURCE=200809L -W -Wall -Wextra -pedantic -g -O0
# Link with liblrt for timing events.
LDFLAGS=-lrt -lm

SOURCES=main.c arraywalk.c util.c csv.c
OBJECTS=$(SOURCES:.c=.o)

# Default target is main, rest is handled by GNU Make builtin rules.
all: main

main: $(SOURCES)

# Some housekeeping does no harm.
clean:
	rm -f main

.PHONY: clean
