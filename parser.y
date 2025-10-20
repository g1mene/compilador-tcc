%{
#include <stdio.h>
#include "parser.tab.h"
#include <string.h>

// Um simbolo da tabela de simbolos é um id e seu endereço
typedef struct {
    char *id;
    int end;
} simbolo;

// Vetor de simbolos (a tabela de simbolos em si)
simbolo tabsimb[1000];
int nsimbs = 0;

// Dado um ID, busca na tabela de simbolos o endereço respectivo
int getendereco(char *id) {
    for (int i=0;i<nsimbs;i++)
        if (!strcmp(tabsimb[i].id, id))
            return tabsimb[i].end;
    printf("Erro semantico"); // imprime o erro semantico
    exit(1);
}

int T=0; // Temporario
int label=0;
int labelfinal=1;

char *operadoratual; // string literal ser usada no operador e em comparacoes

%}

/* o union lista os tipos que o texto/valor de um token pode ter */
%union {
    char *str_val;
    int int_val;
}

/* indicação do tipo do texto/valor de um token, conforme o union. */
%token <str_val> ID
%token <int_val> NUM
%token ATRIB PEV LPAR RPAR AND OR XOR NOT

%type <int_val> expr
%type <int_val> expr_or
%type <int_val> expr_xor
%type <int_val> expr_and
%type <int_val> expr_not


/* mostra detalhes do erro de sintaxe */
%define parse.error verbose

%%

// Regras para ler o exemplo completo
programa : declaracoes
         ;

declaracoes : declaracoes declaracao
            | declaracao
            ;

// Regras gramaticais
declaracao : ID ATRIB expr PEV
                       {
                           printf("Declaracao reconhecida: %s <= ... ;\n", $1);
                       }
                       ;

expr : expr_or;

expr_or : expr_or OR expr_xor
        {
            operadoratual = "or";
            printf("%s %%t%d, %%t%d, %%t%d\n", operadoratual, T, $1, $3);
            $$ = T++;
        }
        |
        expr_xor { $$ = $1;};

expr_xor : expr_xor XOR expr_and
        {
            operadoratual = "xor";
            printf("%s %%t%d, %%t%d, %%t%d\n", operadoratual, T, $1, $3);
            $$ = T++;
        }
        | expr_and { $$ = $1;};

expr_and : expr_and AND expr_not
        {
            operadoratual = "and";
            printf("%s %%t%d, %%t%d, %%t%d\n", operadoratual, T, $1, $3);
            $$ = T++;
        }
        | expr_not { $$ = $1;};

expr_not : NOT expr_not
        {
            operadoratual = "not";
            printf("%s %%t%d, %%t%d\n", operadoratual, T, $2);
            $$ = T++;
        }
        | LPAR expr RPAR { $$ = $2; }
        | ID
        {
            printf("load %%t%d, %s\n", T, $1);
            $$ = T++;
        }
        | NUM
        {
            printf("loadi %%t%d, %d\n", T, $1);
            $$ = T++;
        }
        ;

%%

 extern FILE *yyin;                   // (*) descomente para ler de um arquivo

int main(int argc, char *argv[]) {

    yyin = fopen(argv[1], "r");       // (*)

    yyparse();

    fclose(yyin);                     // (*)

    return 0;
}

void yyerror(char *s) { fprintf(stderr,"ERRO: %s\n", s); }