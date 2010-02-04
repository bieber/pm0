all: vm

vm: codes.h vm.c
	gcc -Wall -Wformat -Werror -o vm vm.c

