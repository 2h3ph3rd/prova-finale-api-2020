ERROR_MESSAGE = && echo "--- TEST SUCCESS ---\n" || echo "!!! --- TEST ERROR --- !!!\n"
TEST_PATH = ./tests/

all: build run

build:
	/usr/bin/gcc -DEVAL -std=gnu11 -O2 -pipe -static -s -o main.out main.c -lm

run:
	./main.out

test: test-writeonly

test-writeonly:
	@echo writeonly 1
	@./main.out < $(TEST_PATH)/writeonly/1_input.txt > $(TEST_PATH)/writeonly/1.txt
	@cmp --silent $(TEST_PATH)/writeonly/1.txt $(TEST_PATH)/writeonly/1_output.txt && echo "writeonly 1" $(ERROR_MESSAGE)

	@./main.out < $(TEST_PATH)/writeonly/2_input.txt > $(TEST_PATH)/writeonly/2.txt
	@echo writeonly 2
	@cmp --silent $(TEST_PATH)/writeonly/2.txt $(TEST_PATH)/writeonly/2.txt $(ERROR_MESSAGE)