#include <stdio.h>
#include <stdlib.h>

#define DEBUG
#define STRING_BUFFER_SIZE 255
#define HISTORY_BUFFER_SIZE 100

// ----- TYPES -----

typedef enum boolean { false, true } t_boolean;

typedef struct command {
    char type;
    int start;
    int end;
    char **data;
    char **prevData;
} t_command;

typedef struct text {
    char **lines;
    int numLines;
} t_text;

// ----- PROTOTYPES -----

// commands
void printCommand(t_text, char *);
void changeCommand(t_text *, char *);

// text manager
char **readText(t_text, int, int);
int writeText(t_text *, char **, int, int);

// i/o utilities
char* readLine();
void printLine(char *);

// read command
int getCommandType(char *line);
void readCommandStartAndEnd(t_command *, char *);
char** readCommandData(t_command);

// other utilities
int stringSize(char* string);
int splitToNumber(char *, char, char);

// ----- COMMANDS -----

void printCommand(t_text text, char *line) {
    t_command command;
    command.type = 'p';
    readCommandStartAndEnd(&command, line);

    if(command.start > text.numLines)
        return;
    if(command.end > text.numLines)
        command.end = text.numLines;
    command.data = readText(text, command.start, command.end);

    for(int i = 0; i < command.end - command.start + 1; i++)
        printf("%s\n", command.data[i]);
}

void changeCommand(t_text *text, char *line)
{
    t_command command;
    command.type = 'c';
    readCommandStartAndEnd(&command, line);
    command.data = readCommandData(command);
    command.prevData = readText(*text, command.start, command.end);
    printf("%s\n", command.data[0]);
    printf("%s\n", command.data[1]);
    printf("%s\n", command.data[2]);
    text -> numLines = writeText(text, command.data, command.start, command.end);
    return;
}

// ----- TEXT MANAGER -----
char **readText(t_text text, int start, int end)
{
    int dim = end - start;
    char **data;
    int j = 0;
    // check if end is bigger than start, otherwise no data to read
    if(dim == -1)
    {
        return NULL;
    }

    data = malloc(sizeof(char *) * dim);

    // check if end not exceed numLines, otherwise end must be decrease to it
    if(end > text.numLines)
        end = text.numLines;
    // read lines
    for(int i = start - 1; i < end; i++)
    {
        data[j] = text.lines[i];
        j++;
    }
    return data;
}
// return new numLines
int writeText(t_text *text, char **newData, int start, int end)
{
    int j = 0;
    int newLines = 0;
    // cannot write out of text
    if(start > text -> numLines + 1)
        return text -> numLines;

    // write lines
    for(int i = start - 1; i < end; i++)
    {
        if(start == text -> numLines + 1)
        {
            newLines++;
            text -> lines[i] = malloc(sizeof(char) * stringSize(newData[j]));
        }
        else
        {
            free(text -> lines[i]);
        }
        text -> lines[i] = newData[j];
        j++;
    }
    newLines = end - text -> numLines;
    if(newLines < 0)
    {
        newLines = 0;
    }
    return text -> numLines + newLines;
}

// ----- I/O UTILITIES -----

char* readLine() {
    int i = 0;
    char c;
    char *buffer = malloc(sizeof(char) * STRING_BUFFER_SIZE);
    int numRealloc = 0;

    c = getchar();

    while (c != '\n') {
        // if current size is bigger than current buffer, increase dimension
        if(i > sizeof(buffer))
        {
            buffer = realloc(buffer, sizeof(buffer) * (numRealloc + 1) * STRING_BUFFER_SIZE);
        }
        buffer[i] = c;
        i++;

        c = getchar();
    }
    // if current size is bigger than current buffer, increase dimension
    if(i > sizeof(buffer))
    {
        buffer = realloc(buffer, sizeof(buffer) * (numRealloc + 1));
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

int getCommandType(char *line)
{
    // command type is always the last char of the line
    return line[stringSize(line) - 1];
}

char **readCommandData(t_command command)
{
    int numLines;
    char *line;
    char **data;
    // num of lines is given by the difference between end and start
    numLines = command.end - command.start + 1;
    // allocate numLines strings
    data = malloc(sizeof(char *) * numLines);
    // read lines
    for(int i = 0; i < numLines; i++)
    {
        line = readLine();
        data[i] = line;
    }
    // read last line with dot
    line = readLine();
    if(line[0] != '.')
    {
        printf("ERROR: change command not have a dot as last line\n");
    }
    return data;
}

void readCommandStartAndEnd(t_command *command, char *line)
{
    char numStr[STRING_BUFFER_SIZE];
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

// ----- OTHER UTILITIES FUNCTIONS -----

int stringSize(char* string) {
    int count = 0;
    while(string[count] != '\0') count++;
    return count;
}

// ----- MAIN -----

int main() {
    t_text text;
    // t_command *history = malloc(sizeof(t_command) * HISTORY_BUFFER_SIZE);
    int commandType;
    char *line;

    text.numLines = 0;

    do
    {
        line = readLine();
        commandType = getCommandType(line);

        switch(commandType) {
            case 'c':
                changeCommand(&text, line);
            break;
            case 'p':
                printCommand(text, line);
            break;
            case 'd':
            break;
            case 'u':
            break;
            case 'r':
            break;
            case 'q':
            break;
            default:
                printf("ERROR: cannot identify command type\n");
                return 0;
            break;
        }
        free(line);
    } while(commandType != 'q');

    return 0;
}