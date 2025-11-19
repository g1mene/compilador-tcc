%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"  // gerado pelo Bison
%}

%option noyywrap

%%

"--".*                  { /* comentário em VHDL, ignorar */ }

"<="                    { return ATRIB; }
";"                     { return PEV; }
"("                     { return LPAR; }
")"                     { return RPAR; }

"AND"                   { return AND; }
"OR"                    { return OR; }
"XOR"                   { return XOR; }
"NOT"                   { return NOT; }

[0-9]+                  { yylval.int_val = atoi(yytext); return NUM; }

[a-zA-Z_][a-zA-Z0-9_]*  { yylval.str_val = strdup(yytext); return ID; }

[ \t\n]+                { /* ignorar espaços/brancos */ }

.                       { printf("Erro léxico: %s\n", yytext); exit(1); }

%%