all: clean scanner parser obj exec
.PHONY: all

build: scanner parser obj exec

clean: 
	rm -f lex.yy.c parser.tab.c parser.tab.h cMinusCompiler *.o *.output

scanner: 
	flex scanner.l

parser: 
	bison -v -d parser.y

obj: 
	gcc -Wall -c *.c -O3

exec: 
	gcc -Wall -g -o cMinusCompiler *.o -ly -O3