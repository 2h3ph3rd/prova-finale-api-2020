/* ----------------------------
 * ---------- HEADER ----------
 * ----------------------------
 */

#include <stdio.h>
#include <stdlib.h>	// to use system()
#include <stdbool.h> // to use bool
#include <string.h> // to use string

#define ADD_BUFFER_SIZE 1000

/* ---------------------------
 * ---------- TYPES ----------
 * ---------------------------
 */

typedef enum color { BLACK, RED } t_color;
typedef enum type { CHANGE, DELETE } t_type;

typedef struct piece {
  int start;
  int end;
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
  t_node *prevNodes;

} t_command;

typedef struct history {
  t_command redoStack;
  t_command undoStack;
  int commandsToUndo;
  bool travelModeActive;
} t_history;

typedef struct table {
  char **lines;
  t_tree *text;
  t_history *history;
} t_table;


/* --------------------------------
 * ---------- PROTOTYPES ----------
 * --------------------------------
 */

// - MAIN UTILITIES
// waitAndClearScreen wait for an interaction by the user and than clear current screen
void waitAndClearScreen();
// waitUserInteraction wait user interaction to continue
void waitUserInteraction();
// clearConsole clear console output
void clearConsole();

// - TREE MENU

// tree_menu submenu to manage tree
void tree_menu(t_tree *);
// inputNewNode get in input a new node key and call tree_insert to add the new node to the tree
void tree_addNewNode(t_tree *);
// deleteNode get in input a key and delete relative node
void tree_deleteNode(t_tree *);
// printTree print all nodes of the given tree (or subtree)
void tree_printTree(t_tree *, t_node *);
// searchNode read in input a key and print all info about the node with the given key
void tree_searchNode(t_tree *, t_node *);
// tree_printNode return a char to indicate color code
char tree_getCharColorCode(t_color);
// tree_printNode print all details of the given node
void tree_printNode(t_tree *, t_node *);
// tree_printInorderTree print all keys in tree in order
void tree_printInorderTree(t_tree *, t_node *);

// - TREE INSERT

// tree_insert add a new node to the tree and fix balancing
void tree_insert(t_tree *, t_node *);
// tree_insertFixupCase1 fix insert when node is root
void tree_insertFixupCase1(t_tree *, t_node *);
// tree_insertFixupCase1 fix insert when node's parent is black
void tree_insertFixupCase2(t_tree *, t_node *);
// tree_insertFixupCase3 fix insert when uncle and parent are red
void tree_insertFixupCase3(t_tree *, t_node *);
// tree_insertFixupCase4 fix insert when node is right child and his parent a left child or vice versa
void tree_insertFixupCase4(t_tree *, t_node *);
// tree_insertFixupCase5 fix insert when node is left child and his parent a left child too or vice versa
void tree_insertFixupCase5(t_tree *, t_node *);

// - TREE DELETE

// tree_delete delete the node given from the tree and call fixup
void tree_delete(t_tree *, t_node *);
// tree_deleteFixupCase1 fix tree balancing after delete a node
// case 1: node is red or root
// it is enough to change its color to BLACK
void tree_deleteFixupCase1(t_tree *, t_node *);
// tree_deleteFixupCase2 fix tree balancing after delete a node
// case 2: node is left (right) child and its sibling is red
// change color of his sibling and parent, than left (right) rotate
void tree_deleteFixupCase2(t_tree *, t_node *);
// tree_deleteFixupCase3 fix tree balancing after delete a node
// case 3: node is black and its sibling is black with both children black
// change color of its sibling to red, so node and sibling subtree is balanced
// now node's parent could be the root and set by before function to red
// must recall delete fixup on it
void tree_deleteFixupCase3(t_tree *, t_node *);
// tree_deleteFixupCase4 fix tree balancing after delete a node
// case 4: node is black and its sibling is black for case 2, with right (left) children black and left (right) children red
// change color of its sibling to red and its sibling's left (right) child to black
// just do a right (left) rotation and go over
void tree_deleteFixupCase4(t_tree *, t_node *);
// tree_deleteFixupCase5 fix tree balancing after delete a node
// case 5: node is black and its sibling is black with right (left) children red
// change color of its sibling as the parent, change parent color to black and change right (left) child color to black
// just do a left (right) rotation and we are finished!
void tree_deleteFixupCase5(t_tree *, t_node *);

// - TREE UTILITIES

// tree_grandfather return a pointer to node's grandparent
t_node *tree_grandparent(t_node *);
// tree_uncle return a pointer to node's uncle
t_node *tree_uncle(t_node *);
// tree_sibling return a pointer to node's sibling
t_node *tree_sibling(t_node *);
// tree_isLeaf return true if the given node is a leaf
bool tree_isLeaf(t_tree *tree, t_node *node);
// tree_rotateLeft do a left rotation of the node given
void tree_rotateLeft(t_tree *, t_node *);
// tree_rotateRight do a right rotation of the node given
void tree_rotateRight(t_tree *, t_node *);
// tree_createNewTree return a new empty tree
t_tree *tree_createNewTree();
// tree_getSortedKeysArray save keys sorted in arr of node subtree, be careful to give an area with enough space
int tree_getSortedKeysArray(t_tree *, t_node *, int, int *);
// tree_getNumNodes return the number of nodes in the given tree or subtree, i indicate the start offset (use 0 if node is root)
int tree_getNumNodes(t_tree *, t_node *, int);
// tree_search return a pointer to the node with the given key, if not exists return NULL
t_node *tree_search(t_tree *, t_node *, int);
// tree_maximumt_tree return maximum node in the subtree from the node given
t_node *tree_maximum(t_tree *, t_node *);
// tree_minimum return minimum node in the subtree from the node given
t_node *tree_minimum(t_tree *, t_node *);
// tree_successor return pointer to the successor node of the node given
t_node *tree_successor(t_tree *, t_node *);
// tree_getNilNode return a pointer to a nil node
t_node *tree_getNilNode();

// - PIECE MENU

void piece_insertText(t_table *);
// piece_appendText add new text in the end
void piece_appendText(t_table *);
// piece_deleteText delete a part of text
void piece_deleteText(t_table *);
// piece_printAllText print all current text
void piece_printAllText(t_table *);

// - PIECE MANAGER

// piece_insert add new piece of text at the position given
void piece_insert(t_table *, t_piece *, int);
// piece_delete delete a piece of text
void piece_delete(t_table *, int, int);
// piece_overwrite overwrite text from the position given
void piece_overwrite(t_table *, t_piece *, int);
// piece_append append new piece of text
void piece_append(t_table *, t_piece *);
// piece_printAll print all current text
void piece_printAll(t_table *);
// piece_printRange print text from start for length given
void piece_printRange(t_table *, int, int);

// - PIECE UTILITIES
// piece_getPieceKey return the correct key of the piece given
int piece_getPieceKey(t_piece *piece);
// piece_getTextLength return the current text length
int piece_getTextLength(t_table *table);
// piece_updateOffset update offsets and keys from the given node
void piece_updateOffset(t_tree *, t_node *);
// piece_createNewNodeFromPiece create a new empty node with the piece given
t_node *piece_createNodeFromPiece(t_table *, t_piece *, int);
// piece_search similar to tree_search but check also for text range
t_node *piece_search(t_table *table, t_node *node, int key);
// piece_saveTextInAddBuffer save string given in add buffer and return its start point
int piece_saveTextInAddBuffer(t_table *, char *);
// piece_inputPiece return a piece read in input and save text in add buffer
t_piece *piece_inputPiece(t_table *);
// piece_newTable generate a new piece table data structure
t_table *piece_newTable();
// piece_getEmptyPiece return an empty piece
t_piece *piece_getEmptyPiece();
// piece_treeInsert do a tree_insert and update offset
void piece_treeInsert(t_table *, t_node *);

/* --------------------------
 * ---------- MAIN ----------
 * --------------------------
 */

int main() {
  int choose;
  t_table *table;

  table = piece_newTable();

  // clearConsole();

  printf("----- A piece table example -----\n");

  do {
    printf("What do you want to do? :-)\n");
    printf("0. Exit\n");
    printf("1. Go to tree menu\n");
    printf("2. Append text\n");
    printf("3. Print all text\n");
    printf("4. Delete text\n");

    printf("Choose: ");
    scanf("%d", &choose);

     switch(choose) {
      case 0:
        printf("Bye :-)\n");
      break;
      case 1:
        printf("\n");
        printf("1. Go to tree menu\n");
        waitAndClearScreen();
        tree_menu(table->tree);
      break;
      case 2:
        printf("\n");
        printf("2. Append text\n");
        piece_appendText(table);
      break;
      case 3:
        printf("\n");
        printf("3. Print all text\n");
        piece_printAllText(table);
      break;
      case 4:
        printf("\n");
        printf("4. Delete text\n");
        piece_deleteText(table);
      break;
      default:
        printf("Bad input\n\n");
      break;
    }
    waitAndClearScreen();
  } while(choose != 0);

  return 0;
}

/* ------------------------------------
 * ---------- MAIN UTILITIES ----------
 * ------------------------------------
 */

// waitAndClearScreen wait for an interaction by the user and than clear current screen
void waitAndClearScreen() {
  waitUserInteraction();
  clearConsole();
}

// waitUserInteraction wait user interaction to continue
void waitUserInteraction() {
  #if defined(_WIN32)
  system("pause");
  #elif defined(__linux__)
  printf("Press something to continue....");
  getchar();
  getchar();
  #endif
}

// clearConsole clear console output
void clearConsole() {
  #if defined(_WIN32)
  system("cls");
  #elif defined(__linux__)
  system("clear");
  #endif
}

/* -------------------------------
 * ---------- TREE MENU ----------
 * -------------------------------
 */

// tree_menu submenu to manage tree
void tree_menu(t_tree *tree) {
  int choose;
  do {
    printf("What do you want to do? :-)\n");
    printf("0. Go back to main menu\n");
    printf("1. Add new node\n");
    printf("2. Delete a node\n");
    printf("3. Search a node\n");
    printf("4. Print all tree\n");

    printf("Choose: ");
    scanf("%d", &choose);

    switch(choose) {
      case 0:
        printf("Going back...\n");
        return;
      break;
      case 1:
        printf("\n");
        printf("1. Add a new node\n");
        tree_addNewNode(tree);
        printf("New node added to the tree!\n\n");
      break;
      case 2:
        printf("\n");
        printf("2. Delete a node\n");
        tree_deleteNode(tree);
      break;
      case 3:
        printf("\n");
        printf("3. Search a node\n");
        tree_searchNode(tree, tree->root);
      break;
      case 4:
        printf("\n");
        printf("4. Print all tree\n");
        tree_printTree(tree, tree->root);
        printf("\n");
      break;
      default:
        printf("Bad input\n\n");
      break;
    }
    waitAndClearScreen();
  } while(choose != 0);
}

// tree_addNewNode get in input a new node key and call tree_insert to add the new node to the tree
void tree_addNewNode(t_tree *tree) {
  t_node *node;
  node = tree_getNilNode();
  printf("Key: ");
  scanf("%d", &node->key);
  tree_insert(tree, node);
  return;
}

// tree_deleteNode get in input a key and delete relative node
void tree_deleteNode(t_tree *tree) {
  t_node *node;
  node = tree_getNilNode();
  printf("Key: ");
  scanf("%d", &node->key);
  node = tree_search(tree, tree->root, node->key);
  if(node == tree->nil) {
    printf("Key not found\n");
  } else {
    printf("Node found\n");
    tree_delete(tree, node);
  }
  return;
}

// tree_printTree print all nodes of the given tree (or subtree)
void tree_printTree(t_tree *tree, t_node *root) {
  printf("Tree has %d nodes\n", tree_getNumNodes(tree, root, 0));
  // check if tree is not empty
  if(tree->root != tree->nil) {
    printf("Root key: %d\n\n", root->key);
    tree_printInorderTree(tree, root);
  }
  return;
}

// tree_searchNode read in input a key and print all info about the node with the given key
void tree_searchNode(t_tree *tree, t_node *root) {
  int key;
  t_node *node;
  printf("Key to search: ");
  scanf("%d", &key);
  node = tree_search(tree, root, key);
  if(node == tree->nil) {
    printf("Key not found\n");
  } else {
    printf("Node found\n");
    tree_printNode(tree, node);
  }
}

// tree_printInorderTree print all keys in tree in order
void tree_printInorderTree(t_tree *tree, t_node *node) {
  if(node != tree->nil) {
    tree_printInorderTree(tree, node->left);

    printf("%d (%c) \t", node->key, tree_getCharColorCode(node->color));

    tree_printInorderTree(tree, node->right);
  }
}
// tree_printNode return a char to indicate color code
char tree_getCharColorCode(t_color color) {
  if(color == BLACK)
    return 'b';
  return 'r';
}

// tree_printNode print all details of the given node
void tree_printNode(t_tree *tree, t_node *node) {
  printf("Key: %d\n", node->key);
  printf("Color: %c\n", tree_getCharColorCode(node->color));

  printf("\n");
  if(node->parent == tree->nil) {
    printf("Root\n");
  } else {
    printf("Parent key: %d\n", node->parent->key);
    printf("Parent color: %c\n", tree_getCharColorCode(node->parent->color));
  }

  printf("\n");
  if(node->left == tree->nil) {
    printf("No left child\n");
  } else {
    printf("Child left key: %d\n", node->left->key);
    printf("Child left color: %c\n", tree_getCharColorCode(node->left->color));
  }

  printf("\n");
  if(node->right == tree->nil) {
    printf("No right child\n");
  } else {
    printf("Child right key: %d\n", node->right->key);
    printf("Child right color: %c\n", tree_getCharColorCode(node->right->color));
  }
  return;
}

/* ---------------------------------
 * ---------- TREE INSERT ----------
 * ---------------------------------
 */

// tree_insert add a new node to the tree and fix balancing
void tree_insert(t_tree *tree, t_node *node) {
  t_node *y = tree->nil;
  t_node *x = tree->root;

  int offset = 0;

  // go to correct position for node
  while(x != tree->nil) {
    y = x;
    if(node->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  // set node parent
  node->parent = y;

  // check if tree is empty
  if(y == tree->nil) {
    // then set node as root
    tree->root = node;
    node->parent = tree->nil;
  } else if(node->key < y->key) {
    // node is the new left child of y
    y->left = node;
  } else {
    // node is the new right child of y
    y->right = node;
  }

  // node is a 'leaf', so set nil as left and right children
  node->left = tree->nil;
  node->right =  tree->nil;
  // temporaly set color of node as red
  node->color = RED;

  // check for rb tree rules broken and fix them
  tree_insertFixupCase1(tree, node);
}

// tree_insertFixupCase1 fix insert when node is root
void tree_insertFixupCase1(t_tree *tree, t_node *node) {
  if (node->parent == tree->nil)
    node->color = BLACK;
  else
    tree_insertFixupCase2(tree, node);
}

// tree_insertFixupCase1 fix insert when node's parent is black
void tree_insertFixupCase2(t_tree *tree, t_node *node) {
  if (node->parent->color == BLACK)
    return; /* Tree is still valid */
  else
    tree_insertFixupCase3(tree, node);
}

// tree_insertFixupCase3 fix insert when uncle and parent are red
void tree_insertFixupCase3(t_tree *tree, t_node *node) {
  t_node *uncle = tree_uncle(node);
  t_node *grandparent = tree_grandparent(node);
  // check uncle color
  // parent is red because previous case check for it
  if(uncle != tree->nil && uncle->color == RED) {
    // in this case invert color
    node->parent->color = BLACK;
    uncle->color = BLACK;
    grandparent->color = RED;
    // grandparent could be the root or a child of a red's father
    // so fixup from the beginning
    tree_insertFixupCase1(tree, grandparent);
  }
  else
    tree_insertFixupCase4(tree, node);
}

// tree_insertFixupCase4 fix insert when node is right child and his parent a left child or vice versa
void tree_insertFixupCase4(t_tree *tree, t_node *node) {
  t_node *grandparent = tree_grandparent(node);
  // check if node is right child of a left child
  if (node == node->parent->right && node->parent == grandparent->left) {
    tree_rotateLeft(tree, node->parent);
    node = node->left;
  } else if (node == node->parent->left && node->parent == grandparent->right) {
    // in this case node is left child of a right child
    tree_rotateRight(tree, node->parent);
    node = node->right;
  }
  tree_insertFixupCase5(tree, node);
}

// tree_insertFixupCase5 fix insert when node is left child and his parent a left child too or vice versa
void tree_insertFixupCase5(t_tree *tree, t_node *node) {
    t_node *grandparent = tree_grandparent(node);
    // change parent and granparent color
    node->parent->color = BLACK;
    grandparent->color = RED;
    if (node == node->parent->left && node->parent == grandparent->left) {
      // Here node is left child and his parent is left child too
      tree_rotateRight(tree, grandparent);
    } else if (node == node->parent->right && node->parent == grandparent->right) {
      // Here node is right child and his parent is right child too
      tree_rotateLeft(tree, grandparent);
    }
}

/* ---------------------------------
 * ---------- TREE DELETE ----------
 * ---------------------------------
 */

// tree_delete delete the node given from the tree and call fixup
void tree_delete(t_tree *tree, t_node *node) {
  t_node *nodeToDelete;
  t_node *substitute;

  // check which node will be deleted
  // if node has at least a subtree
  if(node->left == tree->nil || node->right == tree->nil) {
    // than node will be directly delete
    nodeToDelete = node;
  } else {
    // else node's successor will be removed
    nodeToDelete = tree_successor(tree, node);
  }

  // check if node to delete has a left child
  if(nodeToDelete->left != tree->nil) {
    // than this will be the substitute
    substitute = nodeToDelete->left;
  } else {
    // else the substitute will be the right child
    // warning, this can be a nil node
    substitute = nodeToDelete->right;
  }

  // save correct parent for the substitute
  substitute->parent = nodeToDelete->parent;

  // check if node to delete is the root
  if(nodeToDelete->parent == tree->nil) {
    // than save new root
    tree->root = substitute;
  } else if(nodeToDelete == nodeToDelete->parent->right) {
    // else if node to delete is right child, update parent right child to substitute
    nodeToDelete->parent->right = substitute;
  } else {
    // else node to delete is left child
    nodeToDelete->parent->left = substitute;
  }

  // if node to delete is not equal to node save his key
  // be carefull, we actually want to delete node
  // node to delete is only in a better position for this operation
  if(nodeToDelete != node) {
    node->key = nodeToDelete->key;
  }
  // only when delete a black node tree may need rebalancing
  if(nodeToDelete->color == BLACK)
    tree_deleteFixupCase1(tree, substitute);
  return;
}

// tree_deleteFixupCase1 fix tree balancing after delete a node
// case 1: node is red or root
// it is enough to change its color to BLACK
void tree_deleteFixupCase1(t_tree *tree, t_node *node) {
  if(node->color == RED || node->parent == tree->nil) {
    node->color = BLACK;
    return;
  }
  tree_deleteFixupCase2(tree, node);
  return;
}

// tree_deleteFixupCase2 fix tree balancing after delete a node
// case 2: node is left (right) child and its sibling is red
// change color of his sibling and parent, than left (right) rotate
void tree_deleteFixupCase2(t_tree *tree, t_node *node) {
  t_node *sibling = tree_sibling(node);
  // if node is left child and its sibling red
  if(node == node->parent->left && sibling->color == RED) {
    // than change color and do left rotate
    sibling->color = BLACK;
    node->parent->color = RED;
    tree_rotateLeft(tree, node->parent);
  } else if(node == node->parent->right && sibling->color == RED) {
    // else if node is right child and its sibling red
    sibling->color = BLACK;
    node->parent->color = RED;
    tree_rotateRight(tree, node->parent);
  }
  tree_deleteFixupCase3(tree, node);
  return;
}

// tree_deleteFixupCase3 fix tree balancing after delete a node
// case 3: node is black and its sibling is black with both children black
// change color of its sibling to red, so node and sibling subtree is balanced
// now node's parent could be the root and set by before function to red
// must recall delete fixup on it
void tree_deleteFixupCase3(t_tree *tree, t_node *node) {
  t_node *sibling = tree_sibling(node);

  if(sibling->color == BLACK && sibling->left->color == BLACK && sibling->right->color == BLACK) {
    // change sibling color to red
    sibling->color = RED;
    // recall fixup
    tree_deleteFixupCase1(tree, node->parent);
  }
  tree_deleteFixupCase4(tree, node->parent);
}

// tree_deleteFixupCase4 fix tree balancing after delete a node
// case 4: node is black and its sibling is black for case 2, with right (left) children black and left (right) children red
// change color of its sibling to red and its sibling's left (right) child to black
// just do a right (left) rotation and go over
void tree_deleteFixupCase4(t_tree *tree, t_node *node) {
  t_node *sibling = tree_sibling(node);

  if(sibling->right->color == BLACK) {
    // change sibling color to red
    sibling->left->color = BLACK;
    sibling->color = RED;
    tree_rotateRight(tree, sibling);
  } else if(sibling->left->color == BLACK) {
    // change sibling color to red
    sibling->right->color = BLACK;
    sibling->color = RED;
    tree_rotateRight(tree, sibling);
  }
  tree_deleteFixupCase5(tree, node->parent);
}

// tree_deleteFixupCase5 fix tree balancing after delete a node
// case 5: node is black and its sibling is black with right (left) children red
// change color of its sibling as the parent, change parent color to black and change right (left) child color to black
// just do a left (right) rotation and we are finished!
void tree_deleteFixupCase5(t_tree *tree, t_node *node) {
  t_node *sibling = tree_sibling(node);

  sibling->color = node->parent->color;
  node->parent->color = BLACK;
  if(node == node->parent->left) {
    sibling->right->color = BLACK;
    tree_rotateLeft(tree, node->parent);
  } else {
    sibling->left->color = BLACK;
    tree_rotateLeft(tree, node->parent);
  }

  return;
}

/* ------------------------------------
 * ---------- TREE UTILITIES ----------
 * ------------------------------------
 */

// tree_grandfather return a pointer to node's grandparent
t_node *tree_grandparent(t_node *node) {
  return node->parent->parent;
}

// tree_uncle return a pointer to node's uncle
t_node *tree_uncle(t_node *node) {
  if (node->parent == tree_grandparent(node)->left)
    return tree_grandparent(node)->right;
  else
    return tree_grandparent(node)->left;
}

// tree_sibling return a pointer to node's sibling
t_node *tree_sibling(t_node *node) {
  if (node == node->parent->left)
    return node->parent->right;
  return node->parent->left;
}

// tree_isLeaf return true if the given node is a leaf
bool tree_isLeaf(t_tree *tree, t_node *node) {
  if (node->left == tree->nil && node->right == tree->nil)
    return true;
  return false;
}

// tree_rotateLeft do a left rotation of the node given
void tree_rotateLeft(t_tree *tree, t_node *node) {
  // read node's child for rotation
  t_node *child = node->right;
  // subtree left of child will be the subtree right of node
  node->right = child->left;

  // check if node's child has a left child
  if(child->left != tree->nil) {
    // set parent of left child of node's child as node
    child->left->parent = node;
  }

  // check if node is root
  if(node->parent == tree->nil) {
    // than set child as root
    tree->root = child;
  } else if (node == node->parent->left) {
    // otherwise check if node is a left child
    node->parent->left = child;
  } else {
    // else a right child
    node->parent->right = child;
  }

  // set child parent as node parent
  child->parent = node->parent;
  // set left child of node's child as node
  child->left = node;
  // set node's parent as node's child
  node->parent = child;

  // change offset data
  // new parent will have a bigger left offset
  child->piece->leftOffset = node->piece->leftOffset + node->piece->length;
  // new child not have a right child so left offset is 0
  node->piece->rightOffset = 0;

  return;
}

// tree_rotateRight do a right rotation of the node given
void tree_rotateRight(t_tree *tree, t_node *node)  {
  // read node's child for rotation
  t_node *child = node->left;
  // subtree right of child will be the subtree left of node
  node->left = child->right;

  // check if node's child has a right child
  if(child->right != tree->nil) {
    // set parent of right child of node's child as node
    child->right->parent = node;
  }

  // prepare future child parent
  // check if node is root
  if(node->parent == tree->nil) {
    // than set child as root
    tree->root = child;
  } else if (node == node->parent->left) {
    // otherwise check if node is a left child
    node->parent->left = child;
  } else {
    // else a right child
    node->parent->right = child;
  }

  // set child parent as node parent
  child->parent = node->parent;
  // set right child of node's child as node
  child->right = node;
  // set node's parent as his child
  node->parent = child;

  // change offset data
  // new parent will have a bigger right offset
  child->piece->rightOffset = node->piece->rightOffset + node->piece->length;
  // new child not have a left child so left offset is 0
  node->piece->leftOffset = 0;

  return;
}

// tree_createNewTree return a new empty tree
t_tree *tree_createNewTree() {
  t_tree *tree;
  tree = malloc(sizeof(t_tree));
  tree->nil = tree_getNilNode();
  tree->root = tree->nil;
  return tree;
}

// tree_getSortedKeysArray save keys sorted in arr of node subtree, be careful to give an area with enough space
int tree_getSortedKeysArray(t_tree *tree, t_node *node, int i, int *arr) {
  if(node != tree->nil) {
    i = tree_getSortedKeysArray(tree, node->left, i, arr);
    arr[i] = node->key;
    i++;
    i = tree_getSortedKeysArray(tree, node->right, i, arr);
    return i;
  } else {
    return i;
  }
}

// tree_getNumNodes return the number of nodes in the given tree or subtree, i indicate the start offset (use 0 if node is root)
int tree_getNumNodes(t_tree *tree, t_node *node, int i) {
  if(node != tree->nil) {
    i = tree_getNumNodes(tree, node->left, i);
    i++;
    i = tree_getNumNodes(tree, node->right, i);
    return i;
  } else {
    return i;
  }
}

// tree_search return a pointer to the node with the given key, if not exists return NULL
t_node *tree_search(t_tree *tree, t_node *node, int key) {
  // check if node is a leaf or the node looked for
  if(node == tree->nil || key == node->key)
    return node;
  // check if key is lower of the current node
  if(key < node->key)
    // then go to the left subtree
    return tree_search(tree, node->left, key);
  // otherwise go to the right subtree
  return tree_search(tree, node->right, key);
}


// tree_maximumt_tree return maximum node in the subtree from the node given
t_node *tree_maximum(t_tree *tree, t_node *node) {
  while(node->right != tree->nil) {
    node = node->right;
  }
  return node;
}

// tree_minimum return minimum node in the subtree from the node given
t_node *tree_minimum(t_tree *tree, t_node *node) {
  while(node->left != tree->nil) {
    node = node->left;
  }
  return node;
}

// tree_successor return pointer to the successor node of the node given
t_node *tree_successor(t_tree *tree, t_node *node) {
  t_node *app;
  if(node != tree->nil) {
    return tree_minimum(tree, node->right);
  }
  app = node->parent;
  while(app != tree->nil && node == app->right) {
    node = app;
    app = app->parent;
  }
  return app;
}

// tree_getNilNode return a pointer to a nil node
t_node *tree_getNilNode() {
  t_node *node;
  node = malloc(sizeof(t_node));
  node->right = NULL;
  node->left = NULL;
  node->parent = NULL;
  node->color = BLACK;
  node->key = 0;
  node->piece = piece_getEmptyPiece();
  return node;
}

/* --------------------------------
 * ---------- PIECE MENU ----------
 * --------------------------------
 */

// piece_deleteText delete a part of text
void piece_deleteText(t_table *table) {
  int start, length;

  printf("Start point: ");
  scanf("%d", &start);

  printf("Characters number to delete: ");
  scanf("%d", &length);

  if(length > piece_getTextLength(table)) {
    length = piece_getTextLength(table);
  }

  piece_delete(table, start, length);

  return;
}

// piece_appendText add new text in the end
void piece_appendText(t_table *table) {
  t_piece *piece = piece_inputPiece(table);
  piece_append(table, piece);
}

// piece_printAllText print all current text
void piece_printAllText(t_table *table) {
  printf("\n");
  printf("----- START TEXT -----\n");
  piece_printAll(table);
  printf("----- END TEXT   -----\n\n");
  return;
}

/* -----------------------------------
 * ---------- PIECE MANAGER ----------
 * -----------------------------------
 */

// piece_append append new piece of text
void piece_append(t_table *table, t_piece *piece) {
  int textLength = piece_getTextLength(table);
  t_node *node = piece_createNodeFromPiece(table, piece, textLength);
  piece_treeInsert(table, node);
  return;
}

// piece_printAll print all current text
void piece_printAll(t_table *table) {
  int textLength = piece_getTextLength(table);
  int charRead = 0;
  t_node *node = table->tree->nil;

  while(charRead < textLength) {
    // read node
    node = piece_search(table, table->tree->root, charRead + 1);

    // check where the chars are store
    if(node->piece->source == ADD) {
      for(int i = 0; i < node->piece->length; i++) {
        putchar(table->add[node->piece->start + i]);
      }
    } else {
      for(int i = 0; i < node->piece->length; i++) {
        putchar(table->original[node->piece->start + i]);
      }
    }
    // increase chars read by piece length
    charRead += node->piece->length;
  }
  return;
}

// piece_delete delete text from start for the length given
void piece_delete(t_table *table, int start, int length) {
  t_node *node, *startNode, *endNode;
  int charRead = 0;

  // read first node
  node = piece_search(table, table->tree->root, start);
  startNode = node;
  // check if start node need editing
  if(start != startNode->piece->leftOffset + 1) {
    // create new empty start node
    startNode = tree_getNilNode();
    // set actual start node data
    *startNode = *node;
    // set start node piece
    startNode->piece->length = start - piece_getPieceKey(startNode->piece);
  }

  // check if end is not included in start node
  if(start + length > piece_getPieceKey(node->piece)) {
    charRead += node->piece->length;
    // than search for it
    while(charRead <= length) {
      tree_delete(table->tree, node);
      node = piece_search(table, table->tree->root, start + charRead);
      charRead += node->piece->length;
    }
  }

  // save end node
  // could be the same node used for start
  endNode = node;
  // check if end is not equal to current node end
  if(start + length != piece_getPieceKey(endNode->piece)) {
    // than delete require new node for remaining text
    // create new empty start node
    endNode = tree_getNilNode();
    // set actual start node data
    *endNode = *node;
    // update end node piece
    endNode->piece->start += (start + length) - endNode->piece->leftOffset;
    // before insert, delete current node
    tree_delete(table->tree, node);
    // insert node
    piece_treeInsert(table, endNode);
  }
  // check if startNode has to be entered
  if(start != startNode->piece->leftOffset + 1) {
    piece_treeInsert(table, startNode);
  }

  return;
}

/* -------------------------------------
 * ---------- PIECE UTILITIES ----------
 * -------------------------------------
 */

// piece_getPieceKey return the correct key of the piece given
int piece_getPieceKey(t_piece *piece) {
  return piece->leftOffset + piece->length;
}

// piece_getTextLength return the current text length
int piece_getTextLength(t_table *table) {
  return table->tree->root->piece->leftOffset + table->tree->root->piece->length + table->tree->root->piece->rightOffset;
}

// piece_updateOffset update offsets and keys from the given node
void piece_updateOffset(t_tree *tree, t_node *node) {
  int total;
  if(node != tree->root) {
    // calculate node's total offset
    total = node->piece->rightOffset + node->piece->length + node->piece->leftOffset;
    // if node is left child
    if(node == node->parent->left) {
      // than update left offset
      node->parent->piece->leftOffset = total;
      // change key
      node->parent->key = total;
    } else {
      // else right child
      node->parent->piece->rightOffset = total;
      // key not change, offset is after this node
    }
    piece_updateOffset(tree, node->parent);
  }
  return;
}

// piece_createNodeFromPiece create a new empty node with the piece given
t_node *piece_createNodeFromPiece(t_table *table, t_piece *piece, int offset) {
  int key = offset + piece->length;
  t_node *node = malloc(sizeof(t_node));

  node->key = key;
  node->parent = table->tree->nil;
  node->left = table->tree->nil;
  node->right = table->tree->nil;
  node->piece = piece;

  return node;
}

// piece_search similar to tree_search but check also for text range
t_node *piece_search(t_table *table, t_node *node, int key)  {

  // calculate node's key
  int nodeKey = piece_getPieceKey(node->piece);

  // base cases
  // if node is nil, return nil node, key not found
  if(node == table->tree->nil)
    return node;
  else if(key > node->piece->leftOffset && key <= nodeKey)
    // else check if this node is which we are searching for
    return node;

  // recursive cases
  // check if key is lower of the current node's key
  if(key < nodeKey)
    // then go to the left subtree
    return piece_search(table, node->left, key);
  // otherwise go to the right subtree
  return piece_search(table, node->right, abs(key - nodeKey));
}

// piece_saveTextInAddBuffer save string given in add buffer and return its start point
int piece_saveTextInAddBuffer(t_table *table, char *string) {
  int currentBufferSize = strlen(table->add);
  int newTotal = currentBufferSize + strlen(string);

  // calculate required buffer by num char
  int currentBuffersRequired = currentBufferSize / ADD_BUFFER_SIZE;
  int newBuffersRequired = newTotal / ADD_BUFFER_SIZE;

  // check if new string require more memory
  if(newBuffersRequired > currentBuffersRequired) {
    // than realloc
    table->add = realloc(table->add, sizeof(char) * newBuffersRequired * ADD_BUFFER_SIZE);
  }

  // add string to the end
  strcat(table->add, string);

  // prev length of add indicate also the first free char after current text
  return currentBufferSize;
}

// piece_inputPiece return a piece read in input and save text in add buffer
t_piece *piece_inputPiece(t_table *table) {
  t_piece *piece;
  char *string;

  // allocate piece area
  piece = malloc(sizeof(t_piece));

  // allocate space for string buffer
  // 1000 + '\n' + '\0'
  string = malloc(sizeof(char) * 1002);
  printf("Write text to append (max 1000 char.)\n");
  scanf("%s", string);

  // add a newline to simplify checking
  string[strlen(string)] = '\n';

  // save length
  piece->length = strlen(string);
  // save string and start in buffer
  piece->start = piece_saveTextInAddBuffer(table, string);
  // set source
  piece->source = ADD;

  // string is not need anymore, free it
  free(string);

  return piece;
}

// piece_newTable generate a new piece table data structure
t_table *piece_newTable() {
  t_table *table;
  table = malloc(sizeof(t_table));
  table->add = malloc(sizeof(char) * ADD_BUFFER_SIZE);
  table->original = NULL;
  table->tree = tree_createNewTree();
  return table;
}

// piece_getEmptyPiece return an empty piece
t_piece *piece_getEmptyPiece() {
  t_piece *piece;
  piece = malloc(sizeof(t_piece));
  piece->leftOffset = 0;
  piece->rightOffset = 0;
  piece->length = 0;
  piece->start = 0;
  piece->source = ADD;
  return piece;
}

// piece_treeInsert do a tree_insert and update offset
void piece_treeInsert(t_table *table, t_node *node) {
  tree_insert(table->tree, node);
  piece_updateOffset(table->tree, node);
  return;
}
