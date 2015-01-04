CC=gcc
CFLAGS=-O2 -march=native -w
LDFLAGS=-lm -lalleg

SOURCE=$(wildcard *.c)
DEPENDENCY=$(wildcard *.h) Makefile

all: lacewing

debug: CFLAGS += -DDEBUG -g
debug: lacewing

lacewing: $(SOURCE) $(DEPENDENCY)
	$(CC) $(SOURCE) $(CFLAGS) $(LDFLAGS) -o lacewing
