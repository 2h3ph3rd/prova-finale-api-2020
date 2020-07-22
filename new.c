#include <stdio.h>
#include <stdlib.h>

#define DEBUG
#define MAX_LINE_LENGTH 1024
#define HISTORY_BUFFER_SIZE 100

// ----- TYPES -----

typedef enum boolean { false, true } t_boolean;

typedef struct lines {
    char *line;
    struct lines *next;
} t_line;

typedef struct command {
    char type;
    int start;
    int end;
    char **data;
    char **prevData;
    struct command *next;
} t_command;

typedef struct text {
    t_line *lines;
    int numLines;
} t_text;

typedef struct history {
    // stack
    t_command **pastCommands;
    t_command **futureCommands;
    t_boolean timeTravelMode;
} t_history;

// ----- PROTOTYPES -----

// read command
t_command readCommand();
int getCommandType(char *);
int readCommandStart(t_command, char *);
void readCommandStartAndEnd(t_command *, char *);
char** readCommandData(t_command);

// execute command
void executeCommand(t_command *, t_text *, t_history *);
void printCommand(t_command, t_text);
void changeCommand(t_command *, t_text *);
void deleteCommand(t_command *, t_text *);
void undoCommand(t_command, t_text *, t_history *);
void redoCommand(t_command, t_text *, t_history *);

// update history
t_history createHistory();
t_history updateHistory(t_history *, t_command *);
t_history checkTimeTravelMode(t_history, t_command);
t_history forgetFuture(t_history);
t_history addCommandToHistory(t_history, t_command);

// text manager
t_text createText();
char **readText(t_text, int, int);
int writeText(t_text *, char **, int, int);

// utilities
char* readLine();
void printLine(char *);
int stringSize(char* string);

// ----- READ COMMAND -----

t_command readCommand()
{
    t_command command;
    char *line;

    line = readLine();
    // 1. Read type
    command.type = getCommandType(line);

    // 2. Read interval
    // undo and redo do not have end
    command.end = 0;
    if(command.type == 'c' || command.type == 'd' || command.type == 'p')
        readCommandStartAndEnd(&command, line);
    if(command.type == 'u' || command.type == 'r')
        command.start = readCommandStart(command, line);

    // 3. Read data
    if(command.type == 'c')
        command.data = readCommandData(command);
    return command;
}

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

int readCommandStart(t_command command, char *line)
{
    char numStr[MAX_LINE_LENGTH];
    // counter for line
    int i = 0;
    // counter for numStr
    int j = 0;

    // read start
    while(line[i] != command.type)
    {
        numStr[j] = line[i];
        i++;
        j++;
    }

    numStr[j] = '\0';

    return atoi(numStr);
}

void readCommandStartAndEnd(t_command *command, char *line)
{
    char numStr[MAX_LINE_LENGTH];
    // counter for line
    int i = 0;
    // counter for numStr
    int j = 0;

    // read start
    while(line[i] != ',')
    {
        numStr[j] = line[i];
        i++;
        j++;
    }

    numStr[j] = '\0';
    command -> start = atoi(numStr);

    j = 0;
    // curr char is ','
    i++;
    // read end
    while(line[i] != command -> type)
    {
        numStr[j] = line[i];
        i++;
        j++;
    }
    numStr[j] = '\0';
    command -> end = atoi(numStr);

    return;
}

// ----- EXECUTE COMMAND -----

void executeCommand(t_command *command, t_text *text, t_history *history)
{
    switch(command -> type) {
            case 'p':
                printCommand(*command, *text);
            break;
            case 'c':
                changeCommand(command, text);
            break;
            case 'd':
                deleteCommand(command, text);
            break;
            case 'u':
                undoCommand(*command, text, history);
            break;
            case 'r':
                redoCommand(*command, text, history);
            break;
        }
}

void printCommand(t_command command, t_text text)
{
    int last;

    if(command.start > text.numLines)
        return;
    if(command.end > text.numLines)
        last = text.numLines;
    else
        last = command.end;
    command.data = readText(text, command.start, command.end);

    for(int i = 0; i < last; i++)
        printf("%s\n", command.data[i]);

    for(int i = text.numLines; i < command.end; i++)
        printf(".\n");
}

void changeCommand(t_command *command, t_text *text)
{
    command -> prevData = readText(*text, command -> start, command -> end);
    text -> numLines = writeText(text, command -> data, command -> start, command -> end);
    return;
}

void deleteCommand(t_command *command, t_text *text)
{
    return;
}

void undoCommand(t_command command, t_text *text, t_history *history)
{
    return;
}

void redoCommand(t_command command, t_text *text, t_history *history)
{
    return;
}

// ----- UPDATE HISTORY -----

t_history createHistory()
{
    t_history history;
    history.timeTravelMode = 0;
    return history;
}

t_history updateHistory(t_history *history, t_command *command)
{
    return *history;
}

t_history checkTimeTravelMode(t_history history, t_command command)
{
    return history;
}

t_history forgetFuture(t_history history)
{
    return history;
}

t_history addCommandToHistory(t_history history, t_command command)
{
    return history;
}

// ----- TEXT MANAGER -----

t_text createText()
{
    t_text text;
    text.numLines = 0;
    return text;
}
char **readText(t_text text, int start, int end)
{
    int dim = end - start + 1;
    t_line *curr;
    char **data;
    int lineNumber = 0;
    int readLineNumber = 0;
    // check if end is bigger than start, otherwise no data to read
    if(dim == -1)
    {
        return NULL;
    }

    data = malloc(sizeof(char *) * dim);

    // check if end not exceed numLines, otherwise end must be decrease to it
    if(end > text.numLines)
        end = text.numLines;

    // set first element
    curr = text.lines;

    lineNumber = 1;

    // move to start
    while(lineNumber < start)
    {
        curr = curr->next;
        lineNumber++;
    }
    // read lines
    while(lineNumber < end)
    {
        data[readLineNumber] = curr;
        lineNumber++;
        readLineNumber++;
        curr = curr->next;
    }
    return data;
}

int writeText(t_text *text, char **newData, int start, int end)
{
    int dim = end - start + 1;
    t_line *curr;
    char **data;
    int lineNumber = 0;
    int writeLineNumber = 0;
    int newLines = 0;
    // cannot write out of text or zero lines
    if(start > text -> numLines + 1 || dim < 1)
        return text -> numLines;


    // read first element
    curr = text->lines;
    lineNumber = 1;

    // move to start
    while(lineNumber < start)
    {
        curr = curr->next;
        lineNumber++;
    }
    // write lines
    while(lineNumber < end)
    {
        if(lineNumber > text -> numLines)
        {
            newLines++;
            curr -> next = malloc(sizeof(char) * stringSize(newData[writeLineNumber]));
        }
        else
        {
            free(curr -> line);
        }
        curr -> line = newData[writeLineNumber];
        lineNumber++;
        writeLineNumber++;
        curr = curr->next;
    }
    newLines = end - text -> numLines;
    if(newLines < 0)
    {
        newLines = 0;
    }
    return text -> numLines + newLines;
}

// ----- UTILITIES FUNCTIONS -----

int stringSize(char* string) {
    int count = 0;
    while(string[count] != '\0') count++;
    return count;
}

char* readLine() {
    int i = 0;
    char c;
    char *buffer = malloc(sizeof(char) * MAX_LINE_LENGTH + 1);
    int numRealloc = 0;

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

// ----- MAIN -----

int main() {
    t_text text;
    t_history history = createHistory();
    t_command command;

    /*
        Execution process:
        1. read command
        2. execute command
        3. update history
    */

    command = readCommand();
    while(command.type != 'q')
    {
        command = readCommand();
        executeCommand(&command, &text, &history);
        updateHistory(&history, &command);

        command = readCommand();
    }

    return 0;
}