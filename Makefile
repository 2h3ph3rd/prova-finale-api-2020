all: build run

build:
	/usr/bin/gcc -DEVAL -std=gnu11 -O2 -pipe -static -s -o main.out main.c -lm

run:
	./main.out