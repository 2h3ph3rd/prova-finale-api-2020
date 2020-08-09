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
typedef enum type { CHANGE, DELETE } t_type;

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
