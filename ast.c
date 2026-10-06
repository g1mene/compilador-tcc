#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "ast.h"

static unsigned long nextVariable = 0;

static void *checkedAlloc(size_t size) {
    void *memory = malloc(size);
    if (!memory) {
        fprintf(stderr, "Erro: memoria insuficiente.\n");
        exit(EXIT_FAILURE);
    }
    return memory;
}

static char *copyString(const char *text) {
    char *copy = checkedAlloc(strlen(text) + 1);
    strcpy(copy, text);
    return copy;
}

static Literal literal(const char *variable, int negated) {
    Literal result = { variable, negated };
    return result;
}

static void addClause(Node *node, int size, Literal a, Literal b, Literal c) {
    Clause *clause = &node->clauses[node->numClauses++];
    clause->size = size;
    clause->literals[0] = a;
    clause->literals[1] = b;
    clause->literals[2] = c;
}

/* Cada no guarda uma copia das clausulas dos filhos */
static void applyTseitin(Node *node) {
    int inherited = 0;
    int isNot = strcmp(node->type, "NOT") == 0;
    int isAnd = strcmp(node->type, "AND") == 0;
    int isOr = strcmp(node->type, "OR") == 0;
    int isXor = strcmp(node->type, "XOR") == 0;
    int isDeclaration = strcmp(node->type, "DECLARACAO") == 0;

    for (int i = 0; i < node->numChildren; i++)
        inherited += node->children[i]->numClauses;
    /* No maximo quatro clausulas novas (XOR). */
    node->clauses = checkedAlloc(sizeof(Clause) * (inherited + 4));
    for (int i = 0; i < node->numChildren; i++) {
        Node *child = node->children[i];
        memcpy(node->clauses + node->numClauses, child->clauses,
               sizeof(Clause) * child->numClauses);
        node->numClauses += child->numClauses;
    }

    if (strcmp(node->type, "ID") == 0 || isDeclaration) {
        node->satVariable = copyString(node->value);
    } else if (isNot || isAnd || isOr || isXor ||
               strcmp(node->type, "NUM") == 0) {
        char name[64];
        /* '$' nao faz parte dos identificadores aceitos pelo lexer. */
        snprintf(name, sizeof(name), "$t%lu", ++nextVariable);
        node->satVariable = copyString(name);
    } else {
        return; /* DECLS apenas reune as clausulas. */
    }

    Literal z = literal(node->satVariable, 0);
    Literal nz = literal(node->satVariable, 1);
    if (strcmp(node->type, "NUM") == 0) {
        if (strcmp(node->value, "0") != 0 && strcmp(node->value, "1") != 0) {
            fprintf(stderr, "Erro: Tseitin aceita apenas constantes 0 e 1.\n");
            exit(EXIT_FAILURE);
        }
        addClause(node, 1, strcmp(node->value, "1") == 0 ? z : nz, z, z);
        return;
    }
    if (!(isNot || isAnd || isOr || isXor || isDeclaration)) return;

    int expected = (isNot || isDeclaration) ? 1 : 2;
    if (node->numChildren != expected ||
        !node->children[0]->satVariable ||
        (expected == 2 && !node->children[1]->satVariable)) {
        fprintf(stderr, "Erro: operandos invalidos para %s.\n", node->type);
        exit(EXIT_FAILURE);
    }
    Literal a = literal(node->children[0]->satVariable, 0);
    Literal na = literal(node->children[0]->satVariable, 1);
    if (isNot) { /* z <- NOT a */
        addClause(node, 2, nz, na, z);
        addClause(node, 2, z, a, z);
    } else if (isDeclaration) { 
        addClause(node, 2, nz, a, z);
        addClause(node, 2, z, na, z);
    } else {
        Literal b = literal(node->children[1]->satVariable, 0);
        Literal nb = literal(node->children[1]->satVariable, 1);
        if (isAnd) { /* z <- (a AND b) */
            addClause(node, 2, nz, a, z);
            addClause(node, 2, nz, b, z);
            addClause(node, 3, z, na, nb);
        } else if (isOr) { /* z <-> (a OR b) */
            addClause(node, 2, z, na, z);
            addClause(node, 2, z, nb, z);
            addClause(node, 3, nz, a, b);
        } else { /* z <- (a XOR b) */
            addClause(node, 3, nz, a, b);
            addClause(node, 3, nz, na, nb);
            addClause(node, 3, z, na, b);
            addClause(node, 3, z, a, nb);
        }
    }
}

Node *makeNode(const char *type, const char *value, int numChildren, ...) {
    Node *node = checkedAlloc(sizeof(Node));
    node->type = copyString(type);
    node->value = value ? copyString(value) : NULL;
    node->numChildren = numChildren;
    node->satVariable = NULL;
    node->numClauses = 0;
    node->clauses = NULL;
    node->children = NULL;

    if (numChildren > 0) {
        node->children = checkedAlloc(sizeof(Node *) * numChildren);
        va_list args;
        va_start(args, numChildren);
        for (int i = 0; i < numChildren; i++)
            node->children[i] = va_arg(args, Node *);
        va_end(args);
    }
    applyTseitin(node);
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

void assertTrue(Node *node) {
    if (!node || !node->satVariable) {
        fprintf(stderr, "Erro: no sem resultado booleano.\n");
        exit(EXIT_FAILURE);
    }
    Clause *clauses = checkedAlloc(sizeof(Clause) * (node->numClauses + 1));
    memcpy(clauses, node->clauses, sizeof(Clause) * node->numClauses);
    free(node->clauses);
    node->clauses = clauses;
    Literal z = literal(node->satVariable, 0);
    addClause(node, 1, z, z, z);
}

void printCNF(const Node *node) {
    if (!node) return;
    for (int i = 0; i < node->numClauses; i++) {
        const Clause *clause = &node->clauses[i];
        if (i > 0) printf(" AND\n");
        printf("(");
        for (int j = 0; j < clause->size; j++) {
            if (j > 0) printf(" OR ");
            if (clause->literals[j].negated) printf("NOT ");
            printf("%s", clause->literals[j].variable);
        }
        printf(")");
    }
    printf("\n");
}

/* Numera nomes iguais igualmente, usando busca linear por simplicidade. */
void printDIMACS(const Node *node) {
    if (!node) return;
    const char **variables = checkedAlloc(sizeof(char *) *
                                         ((size_t)node->numClauses * 3 + 1));
    int count = 0;
    for (int i = 0; i < node->numClauses; i++) {
        const Clause *clause = &node->clauses[i];
        for (int j = 0; j < clause->size; j++) {
            const char *name = clause->literals[j].variable;
            int k = 0;
            while (k < count && strcmp(variables[k], name) != 0) k++;
            if (k == count) variables[count++] = name;
        }
    }
    for (int i = 0; i < count; i++)
        printf("c %d %s\n", i + 1, variables[i]);
    printf("p cnf %d %d\n", count, node->numClauses);
    for (int i = 0; i < node->numClauses; i++) {
        const Clause *clause = &node->clauses[i];
        for (int j = 0; j < clause->size; j++) {
            Literal lit = clause->literals[j];
            int k = 0;
            while (k < count && strcmp(variables[k], lit.variable) != 0) k++;
            int number = k + 1;
            printf("%d ", lit.negated ? -number : number);
        }
        printf("0\n");
    }
    free(variables);
}

void freeAST(Node *node) {
    if (!node) return;
    free(node->type);
    free(node->value);
    free(node->clauses);
    free(node->satVariable);
    for (int i = 0; i < node->numChildren; i++)
        freeAST(node->children[i]);
    free(node->children);
    free(node);
}
