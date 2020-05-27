all: debugger

CC=gcc
CLIBS=
CFLAGS=-g -Wall -pedantic -std=c99
LDFLAGS=-g -Wall -pedantic -std=c99

debugger: debugger.o instruction.o printRoutines.o

debugger.o: debugger.c instruction.h
instruction.o: instruction.c instruction.h printRoutines.h
printRoutines.o: printRoutines.c instruction.h printRoutines.h

clean:
	-rm -rf *.o debugger
tidy: clean
	-rm -rf *~
