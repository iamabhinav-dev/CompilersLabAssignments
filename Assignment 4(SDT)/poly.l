%{
#include "poly.tab.h"
#include <string.h>
%}

%%

x {return x;}
[2-9] {yylval.strVal=*yytext; return D;}
\^  { return RTP;}
0  {  return ZERO;}
1  { return ONE;}
\+ { return PLUS;}
- { return MINUS; }
[ \t\n]+    { /* Ignore whitespace */ }

%%

int yywrap(){
    return 1;
}