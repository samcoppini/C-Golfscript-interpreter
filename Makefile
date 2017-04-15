CC=gcc
CFLAGS= -Wall -Wextra -Werror -Wno-comment -std=c11 -pedantic -g
SOURCES=$(wildcard *.c)
OBJS=$(SOURCES:.c=.o)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

all: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o "golf"

clean:
	rm $(OBJS)
