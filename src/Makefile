CC= gcc
CFLAGS= -Wall -g -o
.PHONY: all clean

all: compile interpret


compile: compiler.c ./includes/compiler.h ./includes/global_helper.h ./includes/parser.h ./includes/opcodes.h
	$(CC) $^ $(CFLAGS) $@

interpret: interpreter.c ./includes/interpreter.h ./includes/global_helper.h
	$(CC) $^ $(CFLAGS) $@

clean:
	rm -f compile *scm interpret *.out
