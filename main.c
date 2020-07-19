#include <stdio.h>
#include <stdlib.h>

#define MAX_CHAR 255

#define MAX_LINES_NUMBER 1000

// ----- PROTOTYPES -----

// commands
void printCommand(char**, int, int);

// i/o utilities
char readCommandType();
char* readLine(char*);
void printLine(char*);

// other utilities
int stringSize(char* string);

// ----- COMMANDS FUNCTION -----

void printCommand(char **text, int start, int end) {
    for(int i = start - 1; i < end; i++) {
        printf("%s", text[i]);
    }
}

// ----- I/O UTILITIES FUNCTION -----

char readCommandType() {
    char buffer[MAX_CHAR];
    char* line;

    line = readLine(buffer);

    return line[stringSize(line) - 1];
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

// ----- OTHER UTILITIES FUNCTION -----

int stringSize(char* string) {
    int count = 0;
    while(string[count] != '\0') count++;
    return count;
}

// ----- MAIN -----

int main() {
    char *text[MAX_LINES_NUMBER];
    char commandType;

    commandType = readCommandType();

    while(commandType != 'q') {
        switch(commandType) {
            case 'c':
                // changeCommand(text);
            break;
            case 'p':
                printCommand(text, 1, 3);
            break;
            default:
                printf("ERROR: cannot identify command type");
                return 1;
            break;
        }
        commandType = readCommandType();
    }

    return 0;
}