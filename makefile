all: vm scanner

vm: codes.h vm.c
	gcc -Wall -Wformat -Werror -o vm vm.c

scanner: scanner.c dfa.o
	gcc -Wall -Wformat -Werror -o scanner scanner.c dfa.o

dfa.o: dfa.c dfa.h
	gcc -Wall -Wformat -Werror -c -o dfa.o dfa.c