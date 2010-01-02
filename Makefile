# Copyright (c) 2009 Kross Windz <krosswindz@gmail.com>.
# All rights reserved.
CC=gcc

C99=-std=c99
CFLAGS=-g -Wall -Wextra -O3
GNU=-D_GNU_SOURCE
LIBRARY=-lncurses

TARGETS=timecalc

.c.o:
	$(CC) $(C99) $(CFLAGS) $(GNU) -c $<

all: $(TARGETS)

clean:
	rm -f *.o
	rm -f $(TARGETS)

timecalc: timecalc.o
	$(CC) -o $@ $? $(LIBRARY)
