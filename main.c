/* ----------------------------
 * ---------- HEADER ----------
 * ----------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 1024
#define TEXT_BUFFER_SIZE 1000

/* ---------------------------
 * ---------- TYPES ----------
 * ---------------------------
 */

typedef enum color { BLACK, RED } t_color;
typedef enum type { CHANGE, PRINT, DELETE, REDO, UNDO, QUIT } t_type;

typedef struct piece {
    int start;
    int length;
    int leftOffset;
    int rightOffset;
} t_piece;

typedef struct node {
    struct node *parent;
    struct node *left;
    struct node *right;
    t_color color;
    t_piece *piece;
} t_node;

typedef struct tree {
    t_node *root;
    t_node *nil;
} t_tree;

typedef struct command {
    t_type type;
    int start;
    int length;
    bool deleteWorks;
    t_piece *piece;
    t_piece *prevPieces;
    int numPrevPieces;
    struct command *next;
} t_command;

typedef struct command_stack {
    t_command *commands;
    int num;
} t_command_stack;

typedef struct history {
    t_command_stack *redoStack;
    t_command_stack *undoStack;
    int numCommandsToUndo;
    bool travelModeActive;
} t_history;

typedef struct table {
    char **lines;
    int numLines;
    t_tree *text;
    t_history *history;
} t_table;

/* --------------------------------
 * ---------- PROTOTYPES ----------
 * --------------------------------
 */

// - READ COMMAND

// readCommand read command from input
t_command *readCommand(t_table *);
// getCommandType return command type in given line
t_type getCommandType(char *);
// readCommandStart return command start number
int readCommandStart(t_command *, char *);
// readCommandStartAndEnd set command start and length
void readCommandStartAndEnd(t_command *, char *);
// readCommandData read lines from input, store them in text and return a piece to them
t_piece *readCommandData(t_table *, t_command *);

// - EXECUTE COMMAND

// executeCommand execute the command given
void executeCommand(t_table *, t_command *);
// printCommand print text from start to end (start + length - 1)
void printCommand(t_table *, t_command *);

// - UTILITIES

// checkAndReallocText check text overflow and realloc to bigger area
void checkAndReallocText(t_table *, int );
// readLine read a line from standard input and return a pointer to it
char *readLine();
// getCurrentTextLength return current text length
int getCurrentTextLength(t_table *);

// - TYPES UTILITIES

// createEmptyPiece return a pointer to a new empty piece
t_piece *createEmptyPiece();
// createEmptyCommand return a pointer to a new empty command
t_command *createEmptyCommand();
// createEmptyHistory return a pointer to a new empty history
t_history *createEmptyHistory();
// createNilNode return a pointer to a new nil node
t_node *createNilNode();
// createEmptyTree return a pointer to a new empty tree
t_tree *createEmptyTree();
// createNewEmptyTable return a pointer to a new empty table
t_table *createNewEmptyTable();


/* --------------------------
 * ---------- MAIN ----------
 * --------------------------
 */

int main() {
    t_table *table;
    t_command *command;

    table = createNewEmptyTable();

    command = readCommand(table);

    while (command->type != QUIT) {
        executeCommand(table, command);
        // updateHistory(&history, command);

        command = readCommand(table);
    }

    return 0;
}

/* ----------------------------------
 * ---------- READ COMMAND ----------
 * ----------------------------------
 */

// readCommand read command from input
t_command *readCommand(t_table *table) {
    t_command *command;
    char *line;

    command = createEmptyCommand();

    line = readLine();
    // Read type
    command->type = getCommandType(line);

    // if command type is quit
    if(command->type == QUIT)
        // than nothing to do
        return command;

    // undo and redo do not have end
    if (command->type == UNDO || command->type == REDO) {
        // only start
        command->start = readCommandStart(command, line);
        return command;
    }

    if (command->type == CHANGE || command->type == DELETE || command->type == PRINT)
        readCommandStartAndEnd(command, line);

    // Read data
    if (command->type == CHANGE)
        command->piece = readCommandData(table, command);

    // clear read line
    free(line);

    return command;
}

// getCommandType return command type in given line
t_type getCommandType(char *line) {
    // command type is always the last char of the line
    switch(line[strlen(line) - 1]) {
        case 'c':
            return CHANGE;
        case 'p':
            return PRINT;
        case 'd':
            return DELETE;
        case 'u':
            return UNDO;
        case 'r':
            return REDO;
        case 'q':
            return QUIT;
    }
}

// readCommandData read lines from input, store them in text and return a piece to them
t_piece *readCommandData(t_table *table, t_command *command) {

    char *line;
    t_piece *piece = createEmptyPiece();

    // start cannot be under 0
    if (command->start <= 0) {
        command->start = 1;
    }

    // check for overflow
    checkAndReallocText(table, command->length);

    // save piece info
    piece->start = table->numLines;
    piece->length = command->length;

    // read lines
    for (int i = 0; i < command->length; i++) {
        line = readLine();
        table->lines[table->numLines] = line;
        table->numLines++;
    }

    // read last line with dot
    line = readLine();
    free(line);

    return piece;
}

// readCommandStart return command start number
int readCommandStart(t_command *command, char *line) {
    char numStr[MAX_LINE_LENGTH];
    // counter for line
    int i = 0;
    // counter for numStr
    int j = 0;

    // read start
    while (isdigit(line[i])) {
        numStr[j] = line[i];
        i++;
        j++;
    }

    numStr[j] = '\0';

    return atoi(numStr);
}

// readCommandStartAndEnd set command start and length
void readCommandStartAndEnd(t_command *command, char *line) {
    char numStr[MAX_LINE_LENGTH];
    // counter for line
    int i = 0;
    // counter for numStr
    int j = 0;

    // read start
    while (isdigit(line[i])) {
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
    while (isdigit(line[i])) {
        numStr[j] = line[i];
        i++;
        j++;
    }
    numStr[j] = '\0';
    command->length = atoi(numStr) - command->start + 1;

    return;
}

/* -------------------------------------
 * ---------- EXECUTE COMMAND ----------
 * -------------------------------------
 */

// executeCommand execute the command given
void executeCommand(t_table *table, t_command *command) {
    // if (history->timeTravelMode == true && command->type != 'r' && command->type != 'u') {
    //     if (history->commandsToTravel < 0) {
    //         backToThePast(history, text);
    //     } else if (history->commandsToTravel > 0) {
    //         backToTheFuture(history, text);
    //     }
    //     // otherwise nothing to do
    // }
    switch (command->type) {
        case PRINT:
            printCommand(table, command);
        break;
        case CHANGE:
            // changeCommand(command);
        break;
        case DELETE:
            // deleteCommand(command, text);
        break;
        case UNDO:
            // undoCommand(command, text, history);
        break;
        case REDO:
            // redoCommand(command, text, history);
        break;
    }
}

// printCommand print text from start to end (start + length - 1)
void printCommand(t_table *table, t_command *command) {
    int end = command->start + command->length - 1;
    int textLength = getCurrentTextLength(table);

    // if start is zero, print a line with a dot and continue
    if (command->start == 0) {
        printf(".\n");
        command->start = 1;
        command->length--;
    }

    // check if start is in text, otherwise print only lines with dot
    if (command->start > textLength) {
        for (int i = 0; i < command->length; i++)
            printf(".\n");
    } else {
        // check for overflow
        if (end < textLength) {
            // printText(text, command->start, command->end);
        } else {
            // printText(text, command->start, text->numLines);
            for (int i = textLength; i < end; i++)
                printf(".\n");
        }
    }
}

/* -------------------------------
 * ---------- UTILITIES ----------
 * -------------------------------
 */

// checkAndReallocText check text overflow and realloc to bigger area
void checkAndReallocText(t_table *table, int numLinesToAdd) {
    int buffersRequired = (table->numLines + numLinesToAdd) / TEXT_BUFFER_SIZE;
    // always at least one buffer is allocated
    int buffersAlloc = table->numLines / TEXT_BUFFER_SIZE;
    // check and realloc
    if(buffersRequired > buffersAlloc) {
        table->lines = realloc(table->lines, sizeof(char *) * buffersRequired * TEXT_BUFFER_SIZE);
    }
    return;
}

// readLine read a line from standard input and return a pointer to it
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

// getCurrentTextLength return current text length
int getCurrentTextLength(t_table *table) {
    return table->text->root->piece->leftOffset + table->text->root->piece->length + table->text->root->piece->rightOffset;
}

/* -------------------------------------
 * ---------- TYPES UTILITIES ----------
 * -------------------------------------
 */

// createEmptyPiece return a pointer to a new empty piece
t_piece *createEmptyPiece() {
    t_piece *piece = malloc(sizeof(t_piece));

    piece->start = 0;
    piece->length = 0;
    piece->leftOffset = 0;
    piece->rightOffset = 0;

    return piece;
}

// createEmptyCommand return a pointer to a new empty command
t_command *createEmptyCommand() {
    t_command *command = malloc(sizeof(t_command));

    command->length = 0;
    command->numPrevPieces =  0;
    command->piece = createEmptyPiece();
    command->deleteWorks = true;
    command->next = NULL;

    return command;
}

// createEmptyHistory return a pointer to a new empty history
t_history *createEmptyHistory() {
    t_history *history = malloc(sizeof(t_history));

    history->undoStack = NULL;
    history->redoStack = NULL;
    history->travelModeActive = false;
    history->numCommandsToUndo = 0;

    return history;
}

// createNilNode return a pointer to a new nil node
t_node *createNilNode() {
    t_node *node = malloc(sizeof(t_node));

    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    node->color = BLACK;
    node->piece = createEmptyPiece();

    return node;
}

// createEmptyTree return a pointer to a new empty tree
t_tree *createEmptyTree() {
    t_tree *tree = malloc(sizeof(t_tree));

    tree->nil = createNilNode();
    tree->root = tree->nil;

    return tree;
}

// createNewEmptyTable return a pointer to a new empty table
t_table *createNewEmptyTable() {
    t_table *table = malloc(sizeof(table));

    table->text = createEmptyTree();
    table->history = createEmptyHistory();
    table->lines = malloc(sizeof(char *) * TEXT_BUFFER_SIZE);
    table->numLines = 0;

    return table;
}