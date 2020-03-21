CC		= gcc
DEPS	= main.c vector.c lexer.c file.c parser.c generate.c symbol.c

all:
	$(CC) $(DEPS)	-o napl

backend:
	as a.s -o a.o
	as nrl.s -o nrl.o
	ld a.o nrl.o -o test

clean:
	rm -rf napl test *.o
