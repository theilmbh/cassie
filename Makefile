CC=gcc
CFLAGS=-I.
DEPS = cmplr.h algebra.h derivative.h parse.h simplify.h symbol.h simplify_rne.h simplify.h
OBJ = algebra.o parse.o simplify.o simplify_rne.o symbol.o main.o lex.o derivative.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cassie: $(OBJ) 
	gcc -o $@ $^ $(CFLAGS)
