BUILD_FILE = ./main.out
TEST_MESSAGE = && echo "--- TEST SUCCESS ---\n" || echo "!!! --- TEST ERROR --- !!!\n"
TEST_FOLDER_PATH = ./tests

all: build run

build:
	gcc -o $(BUILD_FILE) main.c -lm

build-prod:
	/usr/bin/gcc -DEVAL -std=gnu11 -O2 -pipe -static -s -o $(BUILD_FILE) main.c -lm

prod:
	python cminify-1.1.1/minifier.py ./main.c > prod.c

run:
	$(BUILD_FILE)

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
	rm *.out