CC= gcc
OBJDIR=./
CFLAGS= -Wall -Wextra -Wpedantic -Wvla -Wshadow -g -fsanitize=address,undefined
.PHONY: all clean

all: $(OBJDIR)compile $(OBJDIR)interpret

$(OBJDIR)compile: compiler.c ./includes/compiler.h ./includes/global_helper.h ./includes/parser.h ./includes/opcodes.h
	$(CC) compiler.c $(CFLAGS) -o $@

interpret: interpreter.c ./includes/interpreter.h ./includes/global_helper.h
	$(CC) interpreter.c $(CFLAGS) -o $@

clean:
	rm -f compile *scm interpret *.out
