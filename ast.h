#ifndef AST_H
#define AST_H

typedef struct Literal {
    /* Aponta para satVariable de um no da mesma arvore. */
    const char *variable;
    int negated;
} Literal;

typedef struct Clause {
    int size;
    Literal literals[3];
} Clause;

typedef struct Node {
    char *type;
    char *value;
    int numChildren;
    struct Node **children;
    char *satVariable;
    int numClauses;
    Clause *clauses;
} Node;

Node *makeNode(const char *type, const char *value, int numChildren, ...);
void printAST(Node *node, int indent);
/* Acrescenta a clausula unitaria que exige o resultado do no verdadeiro.
   Chamar na raiz, depois de construir a arvore. */
void assertTrue(Node *node);
/* Imprime as clausulas armazenadas, incluindo a unidade se adicionada. */
void printCNF(const Node *node);
void freeAST(Node *node);

#endif
