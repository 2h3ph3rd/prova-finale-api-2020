#include <stdio.h>
#include <stdlib.h>	//to use system()
#include <string.h> //to use strcpy()

#define BASE_TESTS_FOLDER "./tests"
#define BUILD_FILE "./main.out"

int main()
{
	char *command;

	char path[100];
	char outputPath[100];
	char inputPath[100];
	char resultPath[100];

    char *folders[100] = {
		"/onlyQuit",
        "/changeAndPrint",
		"/publicTests/writeonly1",
		"/publicTests/writeonly2"
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

	return 0;
}