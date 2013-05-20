PROGRAM = arraywalk
# One could play with compiler optimizations to see whether those have any
# effect.
CFLAGS := -std=c99 -D_POSIX_C_SOURCE=200809L -W -Wall -Wextra -pedantic \
	-O3 -funroll-loops \
	-DNDEBUG \
	-Wconversion -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align \
	-Wwrite-strings -Wno-aggregate-return \
	$(CFLAGS)
LDFLAGS += -lm -lrt

SOURCES = main.c arraywalk.c util.c csv.c options.c
OBJECTS = $(SOURCES:.c=.o)

MAKEDEP = .make.dep

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

# also depend on Makefile because we like to fiddle with it ;-)
%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PROGRAM) $(OBJECTS) $(MAKEDEP)

.PHONY: clean

$(MAKEDEP):
	$(CC) -MM $(CFLAGS) $(SOURCES) > $@

include $(MAKEDEP)
