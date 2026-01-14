CC= gcc
CFLAGS= -o
.PHONY: all clean

all: compile


compile: compiler.c
	$(CC) $^ $(CFLAGS) $@

clean:
	rm -f compile *scm
