#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define HISTORY_BUFFER_SIZE 100
#define TEXT_BUFFER_SIZE 1000

/* ---------------------------
 * ---------- TYPES ----------
 * ---------------------------
 */

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
    t_command *pastCommands;
    int numPastCommands;
    t_command *futureCommands;
    int numFutureCommands;
    t_boolean timeTravelMode;
    int commandsToTravel;
} t_history;

/* --------------------------------
 * ---------- PROTOTYPES ----------
 * --------------------------------
 */

// READ COMMAND

// readCommand read a command from stdin and return it
t_command *readCommand();

// readCommand read command type in the given line
int getCommandType(char *);

// readCommand read command start in the given line
int readCommandStart(t_command, char *);

// readCommand read command start and end in the given line
void readCommandStartAndEnd(t_command *, char *);

// readCommand read command data
t_text readCommandData(t_command);

// EXECUTE COMMAND

// executeCommand execute the given command by call relative function
void executeCommand(t_command *, t_text *, t_history *);

// printCommand execute a print command with data given
void printCommand(t_command *, t_text *);

// changeCommand execute a change command with data given
void changeCommand(t_command *, t_text *);

// deleteCommand execute a delete command with data given
void deleteCommand(t_command *, t_text *);

// undoCommand prepare an undo command for next editing of text and history
void undoCommand(t_command *, t_text *, t_history *);

// redoCommand prepare a redo command for next editing of text and history
void redoCommand(t_command *, t_text *, t_history *);

// UPDATE HISTORY

// createHistory create a new empty history struct
void createHistory(t_history *);

// updateHistory check for update in history after a command execution
void updateHistory(t_history *, t_command *);

// checkForPastChanges check if past is changed than future cannot be restored
void checkForPastChanges(t_history *, t_command *);

// forgetFuture remove all commands stored in future stack
void forgetFuture(t_history *);

// addNewEventToHistory add a new command in history
void addNewEventToHistory(t_history *, t_command *);

// backToTheFuture revert commands to go back to the future
void backToTheFuture(t_history *history, t_text *text);

// backToThePast revert commands to go back to the past
void backToThePast(t_history *history, t_text *text);

// revertCommand revert a command
void revertCommand(t_command *, t_text *, t_boolean);

// revertCommand revert a change command
void revertChange(t_command *command, t_text *text);

// revertCommand revert a delete command
void revertDeleteText(t_text *, t_command *);

// moveEventBetweenStack move a command between first stack to second stack
void moveEventBetweenStack(t_command *, t_command **, t_command **);

// TEXT MANAGER

// createText create a new empty text struct
void createText(t_text *);

// printText print text from start to end
void printText(t_text *, int, int);

// changeText modify text from start with data and length given
t_text changeText(t_text *, t_text, int);

// deleteText delete text from start to end given
void deleteText(t_text *, t_command *);

// readAndOverwriteText overwrite text while reading previous data
t_text readAndOverwriteText(t_text *, t_text, int, int);

// appendText add new text to the end
void appendText(t_text *, t_text, int);

// getTextBuffersRequired get buffers required for a text with length given
int getTextBuffersRequired(int);

// checkAndReallocText check if text needs reallocation
void checkAndReallocText(t_text *, int);

// shiftAndReadText shift text from start to end given while reading previous data
t_text shiftAndReadText(t_text *, int, int);

// UTILITIES

// readLine read a line from stdin
char *readLine();

// printLine print a line to stdout
void printLine(char *);

// swapText swap text struct
void swapText(t_text *, t_text *);

// getEmptyTextStruct return an empty text struct
t_text getEmptyTextStruct();

// freeCommand free all area allocated for a command
void freeCommand(t_command **);

/* --------------------------
 * ---------- MAIN ----------
 * --------------------------
 */

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
    }

    return 0;
}

/* ----------------------------------
 * ---------- READ COMMAND ----------
 * ----------------------------------
 */

// readCommand read a command from stdin and return it
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

// readCommand read command type in the given line
int getCommandType(char *line) {
    // command type is always the last char of the line
    return line[strlen(line) - 1];
}

// readCommand read command data
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

// readCommand read command start in the given line
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

// readCommand read command start and end in the given line
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

/* -------------------------------------
 * ---------- EXECUTE COMMAND ----------
 * -------------------------------------
 */

// executeCommand execute the given command by call relative function
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
    }
}

// printCommand execute a print command with data given
void printCommand(t_command *command, t_text *text) {
    // if start is zero, print a line with a dot and continue
    if (command->start == 0) {
        printLine(".\n");
        command->start = 1;
    }
    // check if start is in text, otherwise online lines with dot
    if (command->start > text->numLines) {
        for (int i = command->start; i < command->end + 1; i++)
            printLine(".\n");
    } else {
        // check for overflow
        if (command->end < text->numLines) {
            printText(text, command->start, command->end);
        } else {
            printText(text, command->start, text->numLines);
            for (int i = text->numLines; i < command->end; i++)
                printLine(".\n");
        }
    }
}

// changeCommand execute a change command with data given
void changeCommand(t_command *command, t_text *text) {
    // start cannot be equal or lower 0
    if (command->start <= 0) {
        command->start = 1;
    }

    command->prevData = changeText(text, command->data, command->start);
}

// deleteCommand execute a delete command with data given
void deleteCommand(t_command *command, t_text *text) {
    // start cannot be greater than num lines
    if(command -> start > text -> numLines) {
        command->deleteWorks = false;
        return;
    }
    // start cannot be equal or lower 0
    if (command->start <= 0) {
        command->start = 1;
    }
    // end cannot be greater than text num lines
    if(command -> end > text -> numLines) {
        command -> end = text -> numLines;
    }
    deleteText(text, command);
}

// undoCommand prepare an undo command for next editing of text and history
void undoCommand(t_command *command, t_text *text, t_history *history) {
    // cannot revert more commands than available
    if (command->start > history->numPastCommands) {
        command->start = history->numPastCommands;
    }
    // set number for revert
    // undo is negative
    history->commandsToTravel -= command->start;
    // modify stack counters
    history->numPastCommands -= command->start;
    history->numFutureCommands += command->start;
    // time travel mode is activated after an undo command
    history->timeTravelMode = true;
}

// redoCommand prepare a redo command for next editing of text and history
void redoCommand(t_command *command, t_text *text, t_history *history) {
    // only when time travel mode is active redo can be done
    if (history->timeTravelMode == true) {
        // cannot revert more commands than available
        if (command->start > history->numFutureCommands) {
            command->start = history->numFutureCommands;
        }
        // set number for revert
        // redo is positive
        history->commandsToTravel += command->start;
        // modify stack counters
        history->numPastCommands += command->start;
        history->numFutureCommands -= command->start;
    }
    return;
}

/* ------------------------------------
 * ---------- UPDATE HISTORY ----------
 * ------------------------------------
 */

// createHistory create a new empty history struct
void createHistory(t_history *history) {
    history->commandsToTravel = 0;
    history->timeTravelMode = false;
    history->futureCommands = NULL;
    history->pastCommands = NULL;
    history->numPastCommands = 0;
    history->numFutureCommands = 0;
    return;
}

// updateHistory check for update in history after a command execution
void updateHistory(t_history *history, t_command *command) {
    checkForPastChanges(history, command);
    addNewEventToHistory(history, command);
    return;
}

// checkForPastChanges check if past is changed than future cannot be restored
void checkForPastChanges(t_history *history, t_command *command) {
    // only when time travel mode is true check if future is change
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

// forgetFuture remove all commands stored in future stack
void forgetFuture(t_history *history) {
    t_command *command, *app;
    command = history->futureCommands;
    while (command != NULL) {
        app = command;
        command = command->next;
        freeCommand(&app);
    }
    history->futureCommands = NULL;
    history->numFutureCommands = 0;
    return;
}

// addNewEventToHistory add a new command in history
void addNewEventToHistory(t_history *history, t_command *command) {
    t_command *newHead;

    // only change and delete commands should be saved
    if (command->type == 'c' || command->type == 'd') {
        newHead = command;
        newHead->next = history->pastCommands;
        history->pastCommands = newHead;
        history->numPastCommands++;
    } else {
        freeCommand(&command);
    }
    return;
}

// backToThePast revert commands to go back to the past
void backToThePast(t_history *history, t_text *text) {
    t_command *command, *app;
    char **strApp;

    history->commandsToTravel *= -1;

    // undo commands in stack
    for (int i = 0; i < history->commandsToTravel; i++) {
        command = history->pastCommands;

        revertCommand(command, text, true);

        moveEventBetweenStack(command, &history->pastCommands, &history->futureCommands);
    }

    history->commandsToTravel = 0;
    return;
}

// backToTheFuture revert commands to go back to the future
void backToTheFuture(t_history *history, t_text *text) {

    t_command *command, *app;

    // redo commands in stack
    for (int i = 0; i < history->commandsToTravel; i++) {
        command = history->futureCommands;

        revertCommand(command, text, false);

        // command go to other stack
        moveEventBetweenStack(command, &history->futureCommands, &history->pastCommands);
    }

    history->commandsToTravel = 0;
    return;
}

// revertCommand revert a command
void revertCommand(t_command *command, t_text *text, t_boolean isUndo) {
    // check command type
    if (command->type == 'c') {
        revertChange(command, text);
    }
    else if (command->type == 'd') {
        if (command->deleteWorks) {
            revertDeleteText(text, command);
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

// revertCommand revert a change command
void revertChange(t_command *command, t_text *text) {
    swapText(text, &command->prevData);
}

// revertCommand revert a delete command
void revertDeleteText(t_text *text, t_command *command) {
    swapText(text, &command->prevData);
}

// moveEventBetweenStack move a command between first stack to second stack
void moveEventBetweenStack(t_command *command, t_command **old, t_command **new) {
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

/* ----------------------------------
 * ---------- TEXT MANAGER ----------
 * ----------------------------------
 */

// createText create a new empty text struct
void createText(t_text *text) {
    text->numLines = 0;
    text->lines = malloc(sizeof(char *) * TEXT_BUFFER_SIZE);
    text->buffersAllocated = 1;
    return;
}

// printText print text from start to end
void printText(t_text *text, int start, int end) {
    int numLinesToPrint = end - start + 1;
    int startOffset = start - 1;

    for(int i = 0; i < numLinesToPrint; i++) {
        printLine(text->lines[startOffset + i]);
    }

    return;
}

// changeText modify text from start with data and length given
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

// deleteText delete text from start to end given
void deleteText(t_text *text, t_command *command) {

    int numLinesToDelete = command->end - command->start + 1;

    // shift lines
    command->prevData = shiftAndReadText(text, command->start, command->end);

    // save new num lines
    text -> numLines = text -> numLines - numLinesToDelete;

    return;
}

// readAndOverwriteText overwrite text while reading previous data
t_text readAndOverwriteText(t_text *text, t_text data, int start, int end) {
    int startOffset = start - 1;
    int numLinesToWrite = 0;
    int currNumLine = 0;
    t_text prevData;
    int dataCounter = 0;

    // check for overflow
    if(end > text->numLines) {
        numLinesToWrite = text->numLines - start + 1;
    } else {
        numLinesToWrite = end - start + 1;
    }

    prevData.numLines = text->numLines;
    prevData.lines = malloc(sizeof(char *) * prevData.numLines);
    prevData.buffersAllocated = 0;

    for(int i = 0; i < startOffset; i++) {
        // save prevData
        prevData.lines[dataCounter] = text->lines[i];
        dataCounter++;
    }

    for(int i = 0; i < numLinesToWrite; i++) {
        currNumLine = startOffset + i;
        // save prevData
        prevData.lines[dataCounter] = text->lines[currNumLine];
        dataCounter++;
        // write data
        text->lines[currNumLine] = data.lines[i];
    }

    startOffset += numLinesToWrite;

    for(int i = 0; i < text->numLines - startOffset; i++) {
        // save prevData
        prevData.lines[dataCounter] = text->lines[startOffset + i];
        dataCounter++;
    }

    return prevData;
}

// appendText add new text to the end
void appendText(t_text *text, t_text data, int numLinesAlreadyWritten) {
    int numLinesToWrite = data.numLines - numLinesAlreadyWritten;
    int startOffset = text->numLines;

    for(int i = 0; i < numLinesToWrite; i++) {
        text->lines[startOffset + i] = data.lines[numLinesAlreadyWritten + i];
        text->numLines++;
    }

    return;
}

// getTextBuffersRequired get buffers required for a text with length given
int getTextBuffersRequired(int numLines) {
    return numLines / TEXT_BUFFER_SIZE + 1;
}

// checkAndReallocText check if text needs reallocation
void checkAndReallocText(t_text *text, int newNumLines) {
    int buffersRequired = getTextBuffersRequired(newNumLines);
    if(buffersRequired > text->buffersAllocated) {
        text->buffersAllocated = buffersRequired;
        text->lines = realloc(text->lines, sizeof(char *) * TEXT_BUFFER_SIZE * text->buffersAllocated);
    }
}

// shiftAndReadText shift text from start to end given while reading previous data
t_text shiftAndReadText(t_text *text, int start, int end)
{
    t_text data;
    int numLinesToDelete = end - start + 1;
    int startOffset = start - 1;
    int endOffset = end;
    int dataCounter = 0;

    data.numLines = text->numLines;
    if(text->buffersAllocated == 0) {
        data.lines = malloc(sizeof(char *) * TEXT_BUFFER_SIZE);
        data.buffersAllocated = 1;
    } else {
        data.lines = malloc(sizeof(char *) * text->buffersAllocated * TEXT_BUFFER_SIZE);
        data.buffersAllocated = text->buffersAllocated;
    }

    for(int i = 0; i < startOffset; i++) {
        // save prev data
        data.lines[dataCounter] = text->lines[i];
        dataCounter++;
    }

    for(int i = 0; i < text->numLines - numLinesToDelete; i++) {
        // save prev data
        data.lines[dataCounter] = text->lines[startOffset + i];
        dataCounter++;
        // overwrite lines from start to end
        text -> lines[startOffset + i] = text->lines[endOffset + i];
    }

    // some rows already shifted
    startOffset = text->numLines - numLinesToDelete;
    for(int i = 0; i < numLinesToDelete; i++) {
        // save prev data
        data.lines[dataCounter] = text->lines[startOffset + i];
        dataCounter++;
    }

    return data;
}

/* ------------------------------------------
 * ---------- UTITILITIES FUNCTION ----------
 * ------------------------------------------
 */

// readLine read a line from stdin
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

// printLine print a line to stdout
void printLine(char *line) {
    int i = 0;
    while (line[i] != '\n' && line[i] != '\0') {
        putchar(line[i]);
        i++;
    }
    putchar('\n');
}

// swapText swap text struct
void swapText(t_text *data1, t_text *data2) {
    t_text app;
    app = *data1;
    *data1 = *data2;
    *data2 = app;
}

// getEmptyTextStruct return an empty text struct
t_text getEmptyTextStruct() {
    t_text data;
    data.lines = NULL;
    data.numLines = 0;
    data.buffersAllocated = 0;
    return data;
}

// freeCommand free all area allocated for a command
void freeCommand(t_command **command) {

    if((*command)->type != 'd') {
        free((*command)->prevData.lines);
        (*command)->prevData.lines = NULL;
    }

    free((*command)->data.lines);
    (*command)->data.lines = NULL;

    free((*command));
    (*command) = NULL;
}
