CC= gcc
CFLAGS= -o
.PHONY: all clean

all: compile interpret


compile: compiler.c
	$(CC) $^ $(CFLAGS) $@

interpret: interpreter.c
	$(CC) $^ $(CFLAGS) $@

clean:
	rm -f compile *scm interpret *.out
