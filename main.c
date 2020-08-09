/* ----------------------------
 * ---------- HEADER ----------
 * ----------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
    int offset;
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
t_command *readCommand(t_table *table);

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

    command = readCommand();

    while (command->type != QUIT) {
        //executeCommand(command, &text, &history);
        // updateHistory(&history, command);

        command = readCommand();
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
        return;

    // undo and redo do not have end
    if (command->type == UNDO || command->type == REDO) {
        // only start
        command->start = readCommandStart(*command, line);
        return;
    }

    if (command->type == CHANGE || command->type == DELETE || command->type == PRINT)
        readCommandStartAndEnd(command, line);

    // 3. Read data
    if (command->type == CHANGE)
        command->lines = readCommandData(*command);

    // clear read line
    free(line);

    return command;
}

// getCommandType return command type in given line
t_type getCommandType(char *line) {
    // command type is always the last char of the line
    switch(line[stringSize(line) - 1]) {
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

/* -------------------------------------
 * ---------- TYPES UTILITIES ----------
 * -------------------------------------
 */

// createEmptyPiece return a pointer to a new empty piece
t_piece *createEmptyPiece() {
    t_piece *piece = malloc(sizeof(t_piece));

    piece->start = 0;
    piece->length = 0;
    piece->offset = 0;

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

    table->text = createNewEmptyTree();
    table->history = createEmptyHistory();
    table->lines = malloc(sizeof(char *) * TEXT_BUFFER_SIZE);
    table->numLines = 0;

    return table;
}