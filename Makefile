BUILD_FILE = ./main.out
TEST_MESSAGE = && echo "--- TEST SUCCESS ---\n" || echo "!!! --- TEST ERROR --- !!!\n"
TEST_BASE_PATH = ./tests

all: build run

build:
	/usr/bin/gcc -DEVAL -std=gnu11 -O2 -pipe -static -s -o $(BUILD_FILE) main.c -lm

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

test: build test-writeonly

TEST_PATH = $(TEST_BASE_PATH)/writeonly

test-writeonly:
	@echo writeonly 1
	@cat $(TEST_PATH)/1_input.txt | $(BUILD_FILE) > $(TEST_PATH)/1.txt
	@cmp --silent $(TEST_PATH)/1.txt $(TEST_PATH)/1_output.txt && echo "writeonly 1" $(TEST_MESSAGE)

	@cat $(TEST_PATH)/2_input.txt | $(BUILD_FILE) > $(TEST_PATH)/2.txt
	@echo writeonly 2
	@cmp --silent $(TEST_PATH)/2.txt $(TEST_PATH)/2_output.txt $(TEST_MESSAGE)