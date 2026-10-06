%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

Node *root;
int yylex(void);
void yyerror(const char *s);
%}

/* Definir os tipos da união */
%union {
    struct Node *node;
    char *str_val;
    int int_val;
}

/* indicação do tipo do texto/valor de um token, conforme o union. */
%token <str_val> ID
%token <int_val> NUM
%token ATRIB AND OR XOR NOT LPAR RPAR PEV

%type <node> declaracoes declaracao expr expr_or expr_xor expr_and expr_not

/* mostra detalhes do erro de sintaxe */
%define parse.error verbose

%%

// Aceita exatamente duas declaracoes
declaracoes
    : declaracao declaracao     { $$ = makeNode("XOR", NULL, 2, $1, $2); root = $$; }
    ;

// Regras gramaticais
declaracao
    : ID ATRIB expr PEV          { $$ = makeNode("DECLARACAO", $1, 1, $3); }
    ;

expr
    : expr_or                    { $$ = $1; }
    ;

expr_or
    : expr_or OR expr_xor        { $$ = makeNode("OR", NULL, 2, $1, $3); }
    | expr_xor                   { $$ = $1; }
    ;

expr_xor
    : expr_xor XOR expr_and      { $$ = makeNode("XOR", NULL, 2, $1, $3); }
    | expr_and                   { $$ = $1; }
    ;

expr_and
    : expr_and AND expr_not      { $$ = makeNode("AND", NULL, 2, $1, $3); }
    | expr_not                   { $$ = $1; }
    ;

expr_not
    : NOT expr_not               { $$ = makeNode("NOT", NULL, 1, $2); }
    | LPAR expr RPAR             { $$ = $2; }
    | ID                         { $$ = makeNode("ID", $1, 0); }
    | NUM                        { 
        char buffer[20];
        sprintf(buffer, "%d", $1);
        $$ = makeNode("NUM", buffer, 0); 
      }
    ;
%%

extern FILE *yyin;

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3 ||
        (argc == 3 && strcmp(argv[2], "--dimacs") != 0)) {
        return EXIT_FAILURE;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror(argv[1]);
        return EXIT_FAILURE;
    }

    int status = yyparse();
    fclose(yyin);
    if (status != 0) {
        fprintf(stderr, "O arquivo deve conter exatamente duas declaracoes validas.\n");
        freeAST(root);
        return EXIT_FAILURE;
    }

    assertTrue(root);
    printDIMACS(root);
    freeAST(root);
    if (fflush(stdout) == EOF || ferror(stdout)) return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

void yyerror(const char *s) { fprintf(stderr,"ERRO: %s\n", s); }
