#ifndef AST_H
#define AST_H

typedef struct Node {
    char *type;
    char *value;
    int numChildren;
    struct Node **children;
} Node;

Node *makeNode(const char *type, const char *value, int numChildren, ...);
void printAST(Node *node, int indent);
void freeAST(Node *node);

#endif
