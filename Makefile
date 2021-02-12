CC=gcc
CFLAGS= -Wall -Wextra -Werror -Wno-comment -std=c11 -pedantic -O3
SOURCES=$(wildcard *.c)
OBJS=$(SOURCES:.c=.o)
TESTS=$(wildcard tests/*.gs)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o "golf"

test: $(TESTS)

tests/%.gs: FORCE
	./golf $@

clean:
	rm $(OBJS)

FORCE:
