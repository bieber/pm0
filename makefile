all: vm

vm: codes.h vm.c
	gcc -o vm vm.c
