#include <stdio.h>
#include <stdlib.h>	//to use system()
#include <string.h> //to use strcpy()

#define BASE_TESTS_FOLDER "./tests"
#define BUILD_FILE "./main.out"
#define MAX_STRING_SIZE 1024

int main()
{
	char command[MAX_STRING_SIZE];

	char path[MAX_STRING_SIZE];
	char outputPath[MAX_STRING_SIZE];
	char inputPath[MAX_STRING_SIZE];
	char resultPath[MAX_STRING_SIZE];

    char *folders[MAX_STRING_SIZE] = {
		"/onlyQuit",
        "/changeAndPrint",
		"/multipleChangeAndPrint",
		"/delete",
		"/undoAndRedo",
		"/prof",
		"/publicTests/writeonly1",
		"/publicTests/writeonly2",
		"/publicTests/bulkreads1",
		"/publicTests/bulkreads2",
		"/publicTests/timeforachange1",
		"/publicTests/timeforachange2",
		"/publicTests/rollingback1",
		"/publicTests/rollingback2",
		"/publicTests/rollercoaster1",
		"/publicTests/rollercoaster2",
		"/publicTests/alterninghistory1",
		"/publicTests/alterninghistory2"
    };

	int i = 0;

	while(folders[i] != NULL) {
		// make path
		strcpy(path, "./tests");
		strcat(path, folders[i]);

		// make input.txt path
		strcpy(inputPath, path);
		strcat(inputPath, "/input.txt");

		// make output.txt path
		strcpy(outputPath, path);
		strcat(outputPath, "/output.txt");

		// make result.txt path
		strcpy(resultPath, path);
		strcat(resultPath, "/result.txt");

		// cat $(TEST_PATH)/1_input.txt | $(BUILD_FILE) > $(TEST_PATH)/1.txt

		// make run command
		strcpy(command, "cat ");
		strcat(command, inputPath);
		strcat(command,  " | ");
		strcat(command, BUILD_FILE);
		strcat(command,  " > ");
		strcat(command,  resultPath);

		system(command);

		// cmp --silent $(TEST_PATH)/1.txt $(TEST_PATH)/1_output.txt && echo "base 1" $(TEST_MESSAGE)
		strcpy(command, "cmp --silent ");
		strcat(command, resultPath);
		strcat(command, " ");
		strcat(command, outputPath);
		strcat(command, " || echo \'----- ERROR IN ");
		strcat(command, folders[i]);
		strcat(command, " -----\'");

		system(command);

		i++;
	}

	printf("Finished all tests\n");

	return 0;
}