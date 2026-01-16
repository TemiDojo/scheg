CC= gcc
CFLAGS= -Wall -g -o
.PHONY: all clean

all: compile interpret


compile: compiler.c ./includes/compiler.h ./includes/global_helper.h ./includes/parser.h
	$(CC) $^ $(CFLAGS) $@

interpret: interpreter.c
	$(CC) $^ $(CFLAGS) $@

clean:
	rm -f compile *scm interpret *.out
