#include <stdio.h>
#include <stdlib.h>

#define DEBUG
#define BUFFER_SIZE 255

// ----- TYPES -----

typedef enum boolean { false, true } t_boolean;

typedef struct command {
    char type;
    int start;
    int end;
    char **data;
} t_command;

typedef struct text {
    char **lines;
    int last_line;
} t_text;

// ----- PROTOTYPES -----

// commands
void printCommand(t_command, t_text);
// void changeCommand(char**, int, int);

// i/o utilities
char* readLine(char *);
void printLine(char *);

// read command
t_command readCommand();
void readCommandType(t_command *, char *);
void readCommandStartAndEnd(t_command *, char *);

// other utilities
int stringSize(char* string);
int splitToNumber(char *, char, char);

// ----- COMMANDS FUNCTION -----

void printCommand(t_command command, t_text text) {
    for(int i = command.start - 1; i < command.end; i++) {
        printf("%s", text.lines[i]);
    }
}

// ----- I/O UTILITIES FUNCTION -----

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

// ----- READ COMMAND -----

t_command readCommand() {
    t_command command;
    char buffer[BUFFER_SIZE];
    char* line;

    line = readLine(buffer);

    readCommandType(&command, line);
    #ifdef DEBUG
    printf("command type: %c\n", command.type);
    #endif

    readCommandStartAndEnd(&command, line);
    #ifdef DEBUG
    printf("command start: %d\n", command.start);
    printf("command end: %d\n", command.end);
    #endif

    return command;
}

void readCommandType(t_command *command, char *line)
{
    // command type is always the last char of the line
    command -> type = line[stringSize(line) - 1];
    return;
}

void readCommandStartAndEnd(t_command *command, char *line)
{
    char numStr[BUFFER_SIZE];
    // counter for line
    int i = 0;
    // counter for numStr
    int j = 0;

    // initialize values
    command -> start = 0;
    command -> end = 0;

    // read start
    while(line[i] != ',' && line[i] != command -> type)
    {
        numStr[j] = line[i];
        i++;
        j++;
    }

    numStr[j] = '\0';
    command -> start = atoi(numStr);

    if(line[i] == ',')
    {
        j = 0;
        i++;
        while(line[i] != command -> type)
        {
            numStr[j] = line[i];
            i++;
            j++;
        }
        numStr[j] = '\0';
        command -> end = atoi(numStr);
    }
    return;
}

// ----- OTHER UTILITIES FUNCTION -----

int stringSize(char* string) {
    int count = 0;
    while(string[count] != '\0') count++;
    return count;
}

// ----- MAIN -----

int main() {
    t_text text;
    t_command command;

    command = readCommand();

    while(command.type != 'q') {
        switch(command.type) {
            case 'c':
                // changeCommand(text);
            break;
            case 'p':
                // printCommand(text);
            break;
            default:
                printf("ERROR: cannot identify command type\n");
                return 1;
            break;
        }
        command = readCommand();
    }

    return 0;
}