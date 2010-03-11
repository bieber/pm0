all: vm scanner parser

vm: codes.h vm.c
	gcc -g -Wall -Wformat -Werror -o vm vm.c

scanner: scanner.c dfa.o
	gcc -g -Wall -Wformat -Werror -o scanner scanner.c dfa.o

dfa.o: dfa.c dfa.h codes.h
	gcc -g -Wall -Wformat -Werror -c -o dfa.o dfa.c

parser: symtable.o parser.c
	gcc -g -Wall -Wformat -Werror -o parser parser.c symtable.o

symtable.o: symtable.h symtable.c
	gcc -g -Wall -Wformat -Werror -c -o symtable.o symtable.c