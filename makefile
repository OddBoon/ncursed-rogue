CC=gcc
DEBUGGER=gcd
OUTPUT=rogue
LIBS=-lcurses
DEBUG=-g
SRCDIR= ./src/
BINDIR= ./bin/
INCLUDE = ./include/
BUILD=$(BINDIR)build/
CFLAGS= $(LIBS) $(DEBUG)


all: rogue

map.o : $(SRCDIR)map.c $(INCLUDE)map.h $(INCLUDE)point.h
	$(CC) $(CFLAGS) -c -I$(INCLUDE) $(SRCDIR)map.c	-o $(BUILD)$@

main.o : $(SRCDIR)main.c $(INCLUDE)rogue.h $(INCLUDE)point.h $(INCLUDE)player.h $(INCLUDE)map.h
	$(CC) $(CFLAGS) -c -I$(INCLUDE) $(SRCDIR)main.c	-o $(BUILD)$@

point.o : $(SRCDIR)point.c $(INCLUDE)point.h
	$(CC) $(CFLAGS) -c -I$(INCLUDE) $(SRCDIR)point.c -o $(BUILD)$@

rogue: main.o point.o map.o
	$(CC) $(DEBUG) $(CFLAGS) $(BUILD)*.o -o $(BINDIR)$(OUTPUT)
debug:
	$(DEBUGGER) $(OUTPUT)
run:
	$(BINDIR)$(OUTPUT)
clean:
	rm -f $(BUILD)*.o $(BINDIR)$(OUTPUT)
