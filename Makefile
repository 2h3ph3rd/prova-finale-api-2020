BUILD_FILE = ./main.out

all: build run

build:
	gcc -o $(BUILD_FILE) main.c -lm

run:
	$(BUILD_FILE)

prod: prod-build prod-run

prod-build:
	/usr/bin/gcc -DEVAL -std=gnu11 -O2 -pipe -static -s -o prod.out prod.c -lm

prod-run:
	./prod.out

memory-leaks:
	gcc main.c -fsanitize=address -g -o main.out
	./main.out

memcheck:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes $(BUILD_FILE)

callgrind:
	valgrind --tool=callgrind --dump-instr=yes --callgrind-out-file=callgrind.out $(BUILD_FILE)
	kcachegrind callgrind.out

massif:
	valgrind --tool=massif --massif-out-file=massif.out $(BUILD_FILE)
	ms_print massif.out

test: build
	gcc -o tests.out tests.c
	./tests.out

clean:
	rm -f *.out
	rm -f main
