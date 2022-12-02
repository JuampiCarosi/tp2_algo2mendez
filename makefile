VALGRIND_FLAGS=--leak-check=full --track-origins=yes --show-reachable=yes --error-exitcode=2 --show-leak-kinds=all --trace-children=yes
CFLAGS =-g -std=c99 -Wall -Wconversion -Wtype-limits -pedantic -Werror -O2
CC = gcc

all: clean valgrind

main: main.c src/*
	$(CC) $(CFLAGS) src/*.c main.c -o main 2>&1

valgrind: main
	valgrind $(VALGRIND_FLAGS) ./main caja1.csv  caja2.csv 2>&1

cajas: main
	./main caja1.csv  caja2.csv 2>&1

valgrind-pruebas: pruebas
	valgrind $(VALGRIND_FLAGS) ./pruebas 2>&1

pruebas: src/*.c pruebas.c
	$(CC) $(CFLAGS) src/*.c pruebas.c -o pruebas 2>&1

clean:
	rm -f ejemplo *.o pruebas 2>&1
