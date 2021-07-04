all: assembler

CC=gcc

LIBS=
CFLAGS=-Os -pipe -s
DEBUGCFLAGS=-Og -pipe -g

INPUT=assembler.c
OUTPUT=assembler

RM=/bin/rm

.PHONY: assembler
assembler:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(CFLAGS)

debug:
	$(CC) $(INPUT) -o $(OUTPUT) $(LIBS) $(DEBUGCFLAGS)

clean:
	if [ -e $(OUTPUT) ]; then $(RM) $(OUTPUT); fi