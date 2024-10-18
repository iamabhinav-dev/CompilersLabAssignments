%{
#include <stdio.h>
#include "expr.tab.h"
%}

%%
"set"           { return SET; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.strval = strdup(yytext); return ID; }
[-]?[0-9]+      { yylval.strval = strdup(yytext); return NUM; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"%"             { return MOD; }
"**"            { return POWER; }
"("             { return LPAREN; }
")"             { return RPAREN; }
[ \t\n]         ; // Ignore whitespace
.               { printf("Unexpected character: %s\n", yytext); }
%%

int yywrap(void) {
    return 1;
}