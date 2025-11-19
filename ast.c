#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ast.h"

Node *makeNode(const char *type, const char *value, int numChildren, ...) {
    Node *node = malloc(sizeof(Node));
    node->type = strdup(type);
    node->value = value ? strdup(value) : NULL;
    node->numChildren = numChildren;

    if (numChildren > 0) {
        node->children = malloc(sizeof(Node *) * numChildren);
        va_list args;
        va_start(args, numChildren);
        for (int i = 0; i < numChildren; i++)
            node->children[i] = va_arg(args, Node *);
        va_end(args);
    } else {
        node->children = NULL;
    }

    return node;
}

void printAST(Node *node, int level) {
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf("%s", node->type);
    if (node->value) printf(" (%s)", node->value);
    printf("\n");

    for (int i = 0; i < node->numChildren; i++)
        printAST(node->children[i], level + 1);
}

void freeAST(Node *node) {
    if (!node) return;
    free(node->type);
    if (node->value) free(node->value);
    for (int i = 0; i < node->numChildren; i++)
        freeAST(node->children[i]);
    free(node->children);
    free(node);
}
