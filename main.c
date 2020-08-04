#include <stdio.h>
#include <stdlib.h>

// #define DEBUG
#define MAX_LINE_LENGTH 1024
#define HISTORY_BUFFER_SIZE 100
#define TEXT_BUFFER_SIZE 100000
#define OFFSET_TOLLERANCE 1000

// ----- TYPES -----

typedef enum boolean {
    false, true
} t_boolean;

typedef struct data {
    char **text;
    int length;
} t_data;

typedef struct command {
    char type;
    int start;
    int end;
    t_data data;
    t_data prevData;
    struct command *next;
    t_boolean deleteWorks;
} t_command;

typedef struct text {
    char **lines;
    int numLines;
    int offset;
} t_text;

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

t_data readCommandData(t_command);

t_data getEmptyDataStruct();

// execute command
void executeCommand(t_command *, t_text *, t_history *);

void printCommand(t_command *, t_text *);

void changeCommand(t_command *, t_text *);

void deleteCommand(t_command *, t_text *);

void undoCommand(t_text *, t_history *);

void redoCommand(t_text *, t_history *);

// update history
void createHistory(t_history *);

void updateHistory(t_history *, t_command *);

void checkForPastChanges(t_history *, t_command *);

void forgetFuture(t_history *);

void addNewEventToHistory(t_history *, t_command *);

void backToTheFuture(t_history *history, t_text *text);

void backToThePast(t_history *history, t_text *text);

void revertChange(t_command *command, t_text *text);

void swipeEventStack(t_command *, t_command **, t_command **);

void returnToStart(t_text *, t_history *);

// text manager
void createText(t_text *);

void printText(t_text *, int, int);

t_data changeText(t_text *, t_data, int);

t_data deleteText(t_text *, int, int);

void revertDeleteText(t_text *, int, int);

int getStartWithOffset(t_text *, int);

// utilities
char *readLine();

void printLine(char *);

int stringSize(char *string);

void swipeData(t_data *, t_data *);

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
        command->data = getEmptyDataStruct();

    // clear read line
    free(line);
    // initialize prevData
    command->prevData.text = NULL;
    command->prevData.length = 0;
    // initialize values
    command->deleteWorks = true;

    return command;
}

int getCommandType(char *line) {
    // command type is always the last char of the line
    return line[stringSize(line) - 1];
}

t_data getEmptyDataStruct() {
    t_data data;
    data.text = NULL;
    data.length = 0;
    return data;
}

t_data readCommandData(t_command command) {
    int numLines;
    char *line;
    t_data data;

    // start cannot be under 0
    if (command.start <= 0) {
        command.start = 1;
    }

    // num of lines is given by the difference between end and start
    numLines = command.end - command.start + 1;

    // allocate numLines strings
    data.text = malloc(sizeof(char *) * numLines + 1);
    // read lines
    for (int i = 0; i < numLines; i++) {
        line = readLine();
        data.text[i] = line;
    }

    // read last line with dot
    line = readLine();
    free(line);

    // save data length
    data.length = numLines;

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
            undoCommand(text, history);
        } else if (history->commandsToTravel > 0) {
            redoCommand(text, history);
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
            if (command->start > history->numPastCommands) {
                command->start = history->numPastCommands;
            }
            history->commandsToTravel -= command->start;
            history->numPastCommands -= command->start;
            history->numFutureCommands += command->start;
            history->timeTravelMode = true;
            break;
        case 'r':
            if (history->timeTravelMode = true) {
                if (command->start > history->numFutureCommands) {
                    command->start = history->numFutureCommands;
                }
                history->commandsToTravel += command->start;
                history->numPastCommands += command->start;
                history->numFutureCommands -= command->start;
            }
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
    command->prevData = deleteText(text, command->start, command->end);
    return;
}

void undoCommand(t_text *text, t_history *history) {
    // set as positive value
    history->commandsToTravel *= -1;

    // cannot undo 0 or lower
    // if(history -> commandsToTravel <= 0)
    //     return;

    // if after undo remain no commands, so simple go back to the start
    if(history->numPastCommands == 0) {
        returnToStart(text, history);
        return;
    }

    // undo commands
    for (int i = 0; i < history->commandsToTravel; i++) {
        // no others commands to undo, so exit
        // if(history -> pastCommands == NULL)
        // {
        //     break;
        // }
        backToThePast(history, text);
    }
    history->commandsToTravel = 0;
    return;
}

void redoCommand(t_text *text, t_history *history) {
    // cannot undo 0 or lower
    // if(history -> commandsToTravel <= 0)
    //     return;
    // undo commands
    for (int i = 0; i < history->commandsToTravel; i++) {
        // no others commands to undo, so exit
        // if(history -> futureCommands == NULL)
        // {
        //     break;
        // }
        backToTheFuture(history, text);
    }
    history->commandsToTravel = 0;
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

    // no commands to do
    // if(history -> pastCommands == NULL)
    //     return;

    command = history->pastCommands;
    // change command to redo
    if (command->type == 'c') {
        revertChange(command, text);
    }
        // delete command to redo
    else if (command->type == 'd') {
        if (command->deleteWorks)
            revertDeleteText(text, command->start, command->end);
    }
    #ifdef DEBUG
    else
    {
        printf("ERROR: try to time travel with a bad command\n");
    }
    #endif

    swipeEventStack(command, &history->pastCommands, &history->futureCommands);
    return;
}

void backToTheFuture(t_history *history, t_text *text) {
    t_command *command, *app;

    // no commands to do
    // if(history -> futureCommands == NULL)
    //     return;

    command = history->futureCommands;
    // change command to redo
    if (command->type == 'c') {
        revertChange(command, text);
    }
    // delete command to redo
    else if (command->type == 'd') {
        if(command->deleteWorks)
            deleteCommand(command, text);
    }
    #ifdef DEBUG
    else
    {
        printf("ERROR: try to time travel with a bad command\n");
    }
    #endif

    // command go to other stack
    swipeEventStack(command, &history->futureCommands, &history->pastCommands);
    return;
}

void revertChange(t_command *command, t_text *text) {

    // restore prev data
    if (command->prevData.text == NULL && command->start == 1) {
        // create new empty text
        free(text->lines);
        createText(text);
    } else {
        changeText(text, command->prevData, command->start);
    }

    // swap
    swipeData(&(command->prevData), &(command->data));
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

void returnToStart(t_text *text, t_history *history)
{
    // move commands to futureStack
    for (int i = 0; i < history->commandsToTravel; i++) {
        swipeData(&(history->pastCommands->data), &(history->pastCommands->prevData));
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
    text->offset = OFFSET_TOLLERANCE;
    text->lines = malloc(sizeof(char *) * TEXT_BUFFER_SIZE);
    return;
}

void printText(t_text *text, int start, int end) {

}

t_data changeText(t_text *text, t_data data, int start) {

}

t_data deleteText(t_text *text, int start, int end) {
}

void revertDeleteText(t_text *text, int start, int end) {
}

int getStartWithOffset(t_text *text, int start) {

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

    line = realloc(line, sizeof(char) * (i + 1));

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

void swipeData(t_data *data1, t_data *data2)
{
    t_data app;
    app = *data1;
    *data1 = *data2;
    *data2 = app;
    return;
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