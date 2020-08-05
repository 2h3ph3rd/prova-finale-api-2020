#include <stdio.h>
#include <stdlib.h>

// #define DEBUG
#define MAX_LINE_LENGTH 1024
#define HISTORY_BUFFER_SIZE 100
#define TEXT_BUFFER_SIZE 1000

// ----- TYPES -----

typedef enum boolean {
    false, true
} t_boolean;

typedef struct text {
    char **lines;
    int numLines;
    int buffersAllocated;
} t_text;

typedef struct command {
    char type;
    int start;
    int end;
    t_text data;
    t_text prevData;
    struct command *next;
    t_boolean deleteWorks;
} t_command;

typedef struct history {
    // stack
    t_command *pastCommands;
    int numPastCommands;
    t_command *futureCommands;
    int numFutureCommands;
    t_boolean timeTravelMode;
    int commandsToTravel;
} t_history;

// ----- PROTOTYPES -----

// read command
t_command *readCommand();

int getCommandType(char *);

int readCommandStart(t_command, char *);

void readCommandStartAndEnd(t_command *, char *);

t_text readCommandData(t_command);

// execute command
void executeCommand(t_command *, t_text *, t_history *);

void printCommand(t_command *, t_text *);

void changeCommand(t_command *, t_text *);

void deleteCommand(t_command *, t_text *);

void undoCommand(t_command *, t_text *, t_history *);

void redoCommand(t_command *, t_text *, t_history *);

// update history
void createHistory(t_history *);

void updateHistory(t_history *, t_command *);

void checkForPastChanges(t_history *, t_command *);

void forgetFuture(t_history *);

void addNewEventToHistory(t_history *, t_command *);

void backToTheFuture(t_history *history, t_text *text);

void backToThePast(t_history *history, t_text *text);

void revertCommand(t_command *, t_text *);

void revertChange(t_command *command, t_text *text);

void swipeEventStack(t_command *, t_command **, t_command **);

void returnToStart(t_text *, t_history *);

// text manager
void createText(t_text *);

void printText(t_text *, int, int);

t_text changeText(t_text *, t_text, int);

void deleteText(t_text *, t_command *);

t_text revertDeleteText(t_text *, t_text);

int getStartWithOffset(t_text *, int);

t_text readAndOverwriteText(t_text *, t_text, int, int);

void appendText(t_text *, t_text, int);

int getTextBuffersRequired(int);

void checkAndReallocText(t_text *, int);

t_text getNewTextArea(int);

// utilities
char *readLine();

void printLine(char *);

int stringSize(char *string);

void swapText(t_text *, t_text *);

t_text getEmptyTextStruct();

// ----- READ COMMAND -----

t_command *readCommand() {
    t_command *command;
    char *line;

    command = malloc(sizeof(t_command));

    line = readLine();
    // 1. Read type
    command->type = getCommandType(line);

    // 2. Read interval
    // undo and redo do not have end
    command->end = 0;
    if (command->type == 'c' || command->type == 'd' || command->type == 'p')
        readCommandStartAndEnd(command, line);
    if (command->type == 'u' || command->type == 'r')
        command->start = readCommandStart(*command, line);

    // 3. Read data
    if (command->type == 'c')
        command->data = readCommandData(*command);
    else
        command->data = getEmptyTextStruct();

    // clear read line
    free(line);
    // initialize prevData
    command->prevData = getEmptyTextStruct();
    // initialize values
    command->deleteWorks = true;
    return command;
}

int getCommandType(char *line) {
    // command type is always the last char of the line
    return line[stringSize(line) - 1];
}

t_text readCommandData(t_command command) {

    char *line;
    t_text data;

    // start cannot be under 0
    if (command.start <= 0) {
        command.start = 1;
    }

    // num of lines is given by the difference between end and start
    data.numLines = command.end - command.start + 1;

    // allocate numLines strings
    data.lines = malloc(sizeof(char *) * data.numLines + 1);
    // read lines
    for (int i = 0; i < data.numLines; i++) {
        line = readLine();
        data.lines[i] = line;
    }

    // read last line with dot
    line = readLine();
    free(line);

    #ifdef DEBUG
        if(line[0] != '.')
        {
            printf("ERROR: change command not have a dot as last line\n");
        }
    #endif
    return data;
}

int readCommandStart(t_command command, char *line) {
    char numStr[MAX_LINE_LENGTH];
    // counter for line
    int i = 0;
    // counter for numStr
    int j = 0;

    // read start
    while (line[i] != command.type) {
        numStr[j] = line[i];
        i++;
        j++;
    }

    numStr[j] = '\0';

    return atoi(numStr);
}

void readCommandStartAndEnd(t_command *command, char *line) {
    char numStr[MAX_LINE_LENGTH];
    // counter for line
    int i = 0;
    // counter for numStr
    int j = 0;

    // read start
    while (line[i] != ',') {
        numStr[j] = line[i];
        i++;
        j++;
    }

    numStr[j] = '\0';
    command->start = atoi(numStr);

    j = 0;
    // curr char is ','
    i++;
    // read end
    while (line[i] != command->type) {
        numStr[j] = line[i];
        i++;
        j++;
    }
    numStr[j] = '\0';
    command->end = atoi(numStr);

    return;
}

// ----- EXECUTE COMMAND -----

void executeCommand(t_command *command, t_text *text, t_history *history) {
    if (history->timeTravelMode == true && command->type != 'r' && command->type != 'u') {
        if (history->commandsToTravel < 0) {
            backToThePast(history, text);
        } else if (history->commandsToTravel > 0) {
            backToTheFuture(history, text);
        }
        // otherwise nothing to do
    }
    switch (command->type) {
        case 'p':
            printCommand(command, text);
            break;
        case 'c':
            changeCommand(command, text);
            break;
        case 'd':
            deleteCommand(command, text);
            break;
        case 'u':
            undoCommand(command, text, history);
            break;
        case 'r':
            redoCommand(command, text, history);
            break;
        #ifdef DEBUG
            default:
                printf("ERROR: cannot identify command type\n");
            break;
        #endif
    }
}

void printCommand(t_command *command, t_text *text) {
    // if start is zero, print a line with a dot and continue
    if (command->start == 0) {
        printf(".\n");
        command->start = 1;
    }
    // check if start is in text, otherwise online lines with dot
    if (command->start > text->numLines) {
        for (int i = command->start; i < command->end + 1; i++)
            printf(".\n");
    } else {
        // check for overflow
        if (command->end < text->numLines) {
            printText(text, command->start, command->end);
        } else {
            printText(text, command->start, text->numLines);
            for (int i = text->numLines; i < command->end; i++)
                printf(".\n");
        }
    }
}

void changeCommand(t_command *command, t_text *text) {
    // start cannot be equal or lower 0
    if (command->start <= 0) {
        command->start = 1;
    }
    // start cannot be greater than last line + 1
    // if(command -> start > text -> numLines + 1)
    //    return;

    command->prevData = changeText(text, command->data, command->start);
    return;
}

void deleteCommand(t_command *command, t_text *text) {
    // start cannot be equal or lower 0
    if (command->start <= 0) {
        command->start = 1;
    }
    // start cannot be greater than num lines
    if(command -> start > text -> numLines)
    {
        command->deleteWorks = false;
        return;
    }
    if(command -> end > text -> numLines)
    {
        command -> end = text -> numLines;
    }
    deleteText(text, command);
    return;
}

void undoCommand(t_command *command, t_text *text, t_history *history) {
    if (command->start > history->numPastCommands) {
        command->start = history->numPastCommands;
    }
    history->commandsToTravel -= command->start;
    history->numPastCommands -= command->start;
    history->numFutureCommands += command->start;
    history->timeTravelMode = true;
    return;
}

void redoCommand(t_command *command, t_text *text, t_history *history) {
    if (history->timeTravelMode == true) {
        if (command->start > history->numFutureCommands) {
            command->start = history->numFutureCommands;
        }
        history->commandsToTravel += command->start;
        history->numPastCommands += command->start;
        history->numFutureCommands -= command->start;
    }
    return;
}

// ----- UPDATE HISTORY -----

void createHistory(t_history *history) {
    history->commandsToTravel = 0;
    history->timeTravelMode = false;
    history->futureCommands = NULL;
    history->pastCommands = NULL;
    history->numPastCommands = 0;
    history->numFutureCommands = 0;
    return;
}

void updateHistory(t_history *history, t_command *command) {
    checkForPastChanges(history, command);
    addNewEventToHistory(history, command);
    return;
}

void checkForPastChanges(t_history *history, t_command *command) {
    // only when true check if future is change
    // than you cannot turn back to your dimension
    if (history->timeTravelMode == true) {
        // only while doing undo and redo you can return in the future
        // modify the past creates a new future
        if (command->type == 'c' || command->type == 'd') {
            history->timeTravelMode = false;
            forgetFuture(history);
        }
    }
    return;
}

void forgetFuture(t_history *history) {
    t_command *command, *app;
    command = history->futureCommands;
    while (command != NULL) {
        app = command;
        command = command->next;
        free(app->data.lines);
        free(app->prevData.lines);
        free(app);
    }
    history->futureCommands = NULL;
    history->numFutureCommands = 0;
    return;
}

void addNewEventToHistory(t_history *history, t_command *command) {
    t_command *newHead;

    // only change and delete commands should be saved
    if (command->type == 'c' || command->type == 'd') {
        newHead = command;
        newHead->next = history->pastCommands;
        history->pastCommands = newHead;
        history->numPastCommands++;
    } else {
        free(command);
    }
    return;
}

void backToThePast(t_history *history, t_text *text) {
    t_command *command, *app;
    char **strApp;

    history->commandsToTravel *= -1;

    // if after undo remain no commands, simple go back to the start
    /*
    if(history->numPastCommands == 0) {
        returnToStart(text, history);
        return;
    }
    */

    // undo commands in stack
    for (int i = 0; i < history->commandsToTravel; i++) {
        command = history->pastCommands;

        revertCommand(command, text);

        swipeEventStack(command, &history->pastCommands, &history->futureCommands);
    }

    history->commandsToTravel = 0;
    return;
}

void backToTheFuture(t_history *history, t_text *text) {

    t_command *command, *app;

    // redo commands in stack
    for (int i = 0; i < history->commandsToTravel; i++) {
        command = history->futureCommands;

        revertCommand(command, text);

        // command go to other stack
        swipeEventStack(command, &history->futureCommands, &history->pastCommands);
    }

    history->commandsToTravel = 0;
    return;
}

void revertCommand(t_command *command, t_text *text) {
    // check command type
    if (command->type == 'c') {
        revertChange(command, text);
    }
    else if (command->type == 'd') {
        if (command->deleteWorks) {
            command->prevData = revertDeleteText(text, command->prevData);
        }
    }
    #ifdef DEBUG
    else
    {
        printf("ERROR: try to time travel with a bad command\n");
    }
    #endif

    return;
}

void revertChange(t_command *command, t_text *text) {

    // restore prev data
    if (command->prevData.lines == NULL && command->start == 1) {
        // create new empty text
        createText(text);
    } else {
        changeText(text, command->prevData, command->start);
        // check if this change append new lines
        if(command->prevData.numLines < command->data.numLines) {
            text->numLines -= command->data.numLines - command->prevData.numLines;
        }
    }

    // swap
    swapText(&(command->prevData), &(command->data));
    return;
}

void swipeEventStack(t_command *command, t_command **old, t_command **new) {
    t_command *app;
    // command go to other stack, so set next as head of commands stack
    app = command->next;
    if (*new == NULL) {
        command->next = NULL;
    } else {
        command->next = *new;
    }
    *new = command;
    // set command as head for the second commands stack
    *old = app;
    return;
}

void returnToStart(t_text *text, t_history *history) {
    // move commands to futureStack
    for (int i = 0; i < history->commandsToTravel; i++) {
        // swipe data only for change commands
        if(history->pastCommands != NULL && history->pastCommands->type == 'c')
            swapText(&(history->pastCommands->data), &(history->pastCommands->prevData));
        swipeEventStack(history->pastCommands, &history->pastCommands, &history->futureCommands);
    }
    // create new empty text
    free(text->lines);
    createText(text);
    history->commandsToTravel = 0;
    return;
}

// ----- TEXT MANAGER -----

void createText(t_text *text) {
    text->numLines = 0;
    text->lines = malloc(sizeof(char *) * TEXT_BUFFER_SIZE);
    text->buffersAllocated = 1;
    return;
}

void printText(t_text *text, int start, int end) {
    int numLinesToPrint = end - start + 1;
    int startOffset = start - 1;

    for(int i = 0; i < numLinesToPrint; i++) {
        printf("%s\n", text->lines[startOffset + i]);
    }

    return;
}

t_text changeText(t_text *text, t_text data, int start) {
    t_text prevData;
    int end = start + data.numLines - 1;

    prevData = getEmptyTextStruct();

    if(start > text->numLines) {
        checkAndReallocText(text, end);
        appendText(text, data, 0);
    } else {
        if(end <= text->numLines) {
            prevData = readAndOverwriteText(text, data, start, end);
        } else {
            prevData = readAndOverwriteText(text, data, start, text->numLines);
            checkAndReallocText(text, end);
            appendText(text, data, text->numLines - start + 1);
        }
    }

    return prevData;
}

void deleteText(t_text *text, t_command *command) {

    int startOffset = 0;
    int currLine = 0;
    int numLinesToShift = text->numLines - command->end;
    int numLinesDeleted = command->end - command->start + 1;
    int numLinesAfterDelete = text->numLines - numLinesDeleted;

    // initialize prevData to allocate all actual text
    command->prevData.numLines = text->numLines;
    command->prevData = getNewTextArea(command->prevData.numLines);

    // save first part of text
    for(int i = 0; i < command->start - 1; i++) {
        // save line
        command->prevData.lines[i] = text->lines[i];
    }

    // delete lines by shift up text
    startOffset = command->start - 1;
    for(int i = 0; i < numLinesToShift; i++) {
        currLine = startOffset + i;
        // save line
        command->prevData.lines[currLine] = text->lines[currLine];
        // shift text
        text->lines[currLine] = text->lines[command->end + i];
    }

    // save last lines
    startOffset = numLinesAfterDelete;
    for(int i = 0; i < numLinesDeleted; i++) {
        currLine = startOffset + i;
        // save line
        command->prevData.lines[currLine] = text->lines[currLine];
    }

    // update num lines
    text->numLines = numLinesAfterDelete;

    return;
}

t_text revertDeleteText(t_text *text, t_text oldText) {
    t_text actualText;

    // save actual text
    actualText.buffersAllocated = text->buffersAllocated;
    actualText.lines = text->lines;
    actualText.numLines = text->numLines;

    // write old text
    text->buffersAllocated = oldText.buffersAllocated;
    text->lines = oldText.lines;
    text->numLines = oldText.numLines;

    return actualText;
}

t_text readAndOverwriteText(t_text *text, t_text data, int start, int end) {
    int startOffset = start - 1;
    int numLinesToWrite = 0;
    int currNumLine = 0;
    t_text prevData;

    // check for overflow
    if(end > text->numLines) {
        numLinesToWrite = text->numLines - start + 1;
    } else {
        numLinesToWrite = end - start + 1;
    }

    prevData.numLines = numLinesToWrite;
    prevData.lines = malloc(sizeof(char *) * prevData.numLines);
    prevData.buffersAllocated = 0;

    for(int i = 0; i < numLinesToWrite; i++) {
        currNumLine = startOffset + i;
        prevData.lines[i] = text->lines[currNumLine];
        text->lines[currNumLine] = data.lines[i];
    }

    return prevData;
}

void appendText(t_text *text, t_text data, int numLinesAlreadyWritten) {
    int numLinesToWrite = data.numLines - numLinesAlreadyWritten;
    int startOffset = text->numLines;

    for(int i = 0; i < numLinesToWrite; i++) {
        text->lines[startOffset + i] = data.lines[numLinesAlreadyWritten + i];
        text->numLines++;
    }

    return;
}

int getTextBuffersRequired(int numLines) {
    return numLines / TEXT_BUFFER_SIZE + 1;
}

void checkAndReallocText(t_text *text, int newNumLines) {
    int buffersRequired = getTextBuffersRequired(newNumLines);
    if(buffersRequired > text->buffersAllocated) {
        text->buffersAllocated++;
        text->lines = realloc(text->lines, sizeof(char *) * TEXT_BUFFER_SIZE * text->buffersAllocated);
    }
}

t_text getNewTextArea(int numLines) {
    t_text newText;

    newText.numLines = numLines;
    newText.buffersAllocated = getTextBuffersRequired(newText.numLines);
    newText.lines = malloc(sizeof(char *) * newText.buffersAllocated * TEXT_BUFFER_SIZE);

    return newText;
}

// ----- UTILITIES FUNCTIONS -----

int stringSize(char *string) {
    int count = 0;
    while (string[count] != '\0') count++;
    return count;
}

char *readLine() {
    int i;
    char c;
    char *line = malloc(sizeof(char) * (MAX_LINE_LENGTH + 1));

    c = getchar();
    i = 0;

    while (c != '\n') {
        line[i] = c;
        i++;

        c = getchar();
    }
    line[i] = '\0';
    i++;

    line = realloc(line, sizeof(char) * i);

    return line;
}

void printLine(char *line) {
    int i = 0;
    while (line[i] != '\0') {
        putchar(line[i]);
        i++;
    }
    putchar('\n');
}

void swapText(t_text *data1, t_text *data2) {
    t_text app;
    app = *data1;
    *data1 = *data2;
    *data2 = app;
    return;
}

t_text getEmptyTextStruct() {
    t_text data;
    data.lines = NULL;
    data.numLines = 0;
    data.buffersAllocated = 0;
    return data;
}

// ----- MAIN -----

int main() {
    t_text text;
    t_history history;
    t_command *command;

    createText(&text);
    createHistory(&history);

    /*
        Execution process:
        1. read command
        2. execute command
        3. update history
    */

    command = readCommand();

    while (command->type != 'q') {
        executeCommand(command, &text, &history);
        updateHistory(&history, command);

        command = readCommand();
        #ifdef DEBUG
                printf("Command type: %c\n", command.type);
                printf("Command start: %d\n", command.start);
                printf("Command end: %d\n", command.end);
                printf("Text length: %d\n", text.numLines);
        #endif
    }

    return 0;
}