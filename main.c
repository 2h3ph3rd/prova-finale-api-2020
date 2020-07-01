#include <stdio.h>
#include <stdlib.h>

#define MAX_CHAR 255

void print(char**, int, int);
char* readLine(char*);
void printLine(char*);

void print(char **text, int start, int end) {
    for(int i = start; i < end; i++) {
        printf("%s/n", text[i]);
    }
}

char* readLine(char* buffer) {
    int i = 0;
    char c;

    c = getchar();

    while (c != '\n') {
        buffer[i] = c;
        i++;

        c = getchar();
    }

    buffer[i] = '\0';

    return buffer;
}

void printLine(char* line) {
    int i = 0;
    while(line[i] != '\0') {
        putchar(line[i]);
        i++;
    }
    putchar('\n');
}

int stringSize(char* string) {
    int count = 0;
    while(string[count] != '\0') count++;
    return count;
}

char readCommandType() {
    char buffer[MAX_CHAR];
    char* line;

    line = readLine(buffer);
    printLine(line);

    return line[0];
}

void printText(char** text) {
    int i, j;
    char* line;

    i = 0;
    while(text[i] != NULL) {

        j = 0;
        line = text[i];
        while (line[j] != '\0') {
            putchar(line[j]);
            j++;
        }

        i++;
    }
}

int main() {
    char** text;
    char commandType;
    int start, end;

    char str1 = "Test test 1\0";
    char str2 = "Test test 2\0";
    char str3 = "Test test 3\0";

    text = malloc(sizeof(char*) * 4);
    text[0] = &str1;
    text[1] = &str2;
    text[2] = &str3;
    text[3] = NULL;

    printText(text);
    /*

    while(commandType != 'q') {
        commandType = readCommandType();
    }

    return 0;

    
    if(line[0] == 'q') {
        return 0;
    }

    do {

        command = line[3];

        switch(command) {
            case 'c':
                start = line[0];
                end = line[1];
                while (line[0] != '.') {
                    line = readLine();
                    text[start] = line;
                    start++;
                }
            break;
            case 'p':
                print(text, (int) line[0], (int) line[2]);
            break;
        }

        line = readLine();
        printLine(line);

    } while (line[0] != '0');

    return 0;
    */
}