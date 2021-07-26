all: cpu

CC=gcc

LIBS=
CFLAGS=-Os -pipe -s -Wno-format-extra-args
DEBUGCFLAGS=-Og -pipe -g

INPUT=cpu.c
OUTPUT=cpu

RM=/bin/rm

.PHONY: cpu
cpu:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

debug:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi
