#include <stdio.h>
#include <stdlib.h>	// to use system()
#include <string.h> // to use strcpy()
#include <dirent.h> // to use opendir()

// #define DEBUG

#define BASE_TESTS_FOLDER "./tests"
#define BUILD_FILE "./main.out"
#define MAX_STRING_SIZE 1000
#define MAX_ARRAY_SIZE 1000

typedef enum boolean { false, true } t_boolean;

typedef struct text
{
	char **data;
	int len;
} t_text;

void executeTest(char *, char *, char *, char *);
t_text getTestFolders();
t_boolean isDir(char *);
char *concatFoldersPath(char *, char *);
char **getBaseFolders();
t_text getSubfolders(char **);
char *getInputPath(char *);
char *getOutputPath(char *);
char *getResultPath(char *);

void executeTest(char *testName, char *inputPath, char *outputPath, char *resultPath)
{
	char *command;

	command = malloc(sizeof(char) * MAX_STRING_SIZE);

	// cat $(TEST_PATH)/1_input.txt | $(BUILD_FILE) > $(TEST_PATH)/1.txt
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
	strcat(command, testName);
	strcat(command, " -----\'");

	system(command);
}


t_text getTestFolders()
{
    return getSubfolders(getBaseFolders());
}

t_boolean isDir(char *name)
{
	return strcmp(name, ".") != 0 && strcmp(name, "..") != 0;
}

char *concatFoldersPath(char *folder1, char *folder2)
{
	char *folder;
	folder = malloc(sizeof(char) * MAX_STRING_SIZE);
	strcpy(folder, folder1);
	strcat(folder, "/");
	strcat(folder, folder2);
	return folder;
}

char **getBaseFolders()
{
	DIR *openFolder;
	struct dirent *entry;
	char **foldersPath;

	foldersPath = malloc(sizeof(char *) * MAX_ARRAY_SIZE);

	#ifdef DEBUG
		printf("Reading base folders\n");
	#endif

	openFolder = opendir(BASE_TESTS_FOLDER);
	if(openFolder == NULL)
	{
		puts("Unable to read base directory");
		return NULL;
	}

	for(int i = 0; entry = readdir(openFolder); i++)
	{
		// avoid parent and local dir
		if(isDir(entry -> d_name))
		{
			#ifdef DEBUG
				printf("%s - ", entry -> d_name);
			#endif
			foldersPath[i] = concatFoldersPath(BASE_TESTS_FOLDER, entry -> d_name);
			#ifdef DEBUG
				printf("%s\n", foldersPath[i]);
			#endif
		}
		else
		{
			i--;
		}
	}

	closedir(openFolder);

	#ifdef DEBUG
		printf("End reading base folders\n\n\n");
	#endif

	return foldersPath;
}

t_text getSubfolders(char **folders)
{
	DIR *folder;
	struct dirent *entry;
	t_text foldersPath;

	#ifdef DEBUG
		printf("Reading subfolders\n");
	#endif

	foldersPath.data = malloc(sizeof(char *) * MAX_ARRAY_SIZE);
	foldersPath.len = 0;

	for(int i = 0; folders[i] != NULL; i++)
	{
		#ifdef DEBUG
			printf("Try to open %s\n", folders[i]);
		#endif

		folder = opendir(folders[i]);

		if(folder == NULL)
		{
			puts("Unable to read subfolder directory");
			return foldersPath;
		}

		while(entry = readdir(folder))
		{
			// avoid parent and local dir
			if(isDir(entry -> d_name))
			{
				#ifdef DEBUG
					printf("%s - ", entry -> d_name);
				#endif
				foldersPath.data[foldersPath.len] = concatFoldersPath(folders[i], entry -> d_name);
				foldersPath.len++;
				#ifdef DEBUG
					printf("%s\n", foldersPath.data[foldersPath.len - 1]);
				#endif
			}
		}

		closedir(folder);

		#ifdef DEBUG
			printf("End reading of %s\n\n\n", folders[i]);
		#endif
	}

	#ifdef DEBUG
		printf("End reading subfolders\n\n\n");
	#endif

	return foldersPath;
}

char *getInputPath(char *folder)
{
	char *path;

	path = malloc(sizeof(char) * MAX_STRING_SIZE);

	strcpy(path, folder);
	strcat(path, "/input.txt");

	return path;
}

char *getOutputPath(char *folder)
{
	char *path;

	path = malloc(sizeof(char) * MAX_STRING_SIZE);

	strcpy(path, folder);
	strcat(path, "/output.txt");

	return path;
}

char *getResultPath(char *folder)
{
	char *path;

	path = malloc(sizeof(char) * MAX_STRING_SIZE);

	strcpy(path, folder);
	strcat(path, "/result.txt");

	return path;
}

int main()
{
	t_text folders = getTestFolders();

	if(folders.len == 0)
	{
		printf("No tests found\n");
	}
	else
	{
		for(int i = 0; i < folders.len; i++)
		{
			#ifdef DEBUG
				printf("Executing tests in %s\n", folders.data[i]);
				printf("Input path %s\n", getInputPath(folders.data[i]));
				printf("Output path %s\n", getOutputPath(folders.data[i]));
				printf("Result path %s\n", getResultPath(folders.data[i]));
			#endif
			executeTest(folders.data[i], getInputPath(folders.data[i]), getOutputPath(folders.data[i]), getResultPath(folders.data[i]));
			#ifdef DEBUG
				printf("Finished tests in %s\n\n\n", folders.data[i]);
			#endif
		}

		printf("Finished all tests\n");
	}

	return 0;
}
