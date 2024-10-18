%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intcodegen.h"

extern int yylex();
extern int yyparse();
extern FILE* yyin;
void yyerror(const char* s);
%}



%union {
    char* strval;
}

%token <strval> NUM
%token <strval> ID 
%token SET PLUS MINUS TIMES DIVIDE MOD POWER LPAREN RPAREN 

%type <strval> ARG EXPR 
%type <strval> OP

%%

PROGRAM: STMT PROGRAM 
       | STMT
       ;

STMT: SETSTMT
    | EXPRSTMT
    ;

SETSTMT: LPAREN SET ID NUM RPAREN { 
       // printf("DEBUG: Processing SETSTMT: set ID NUM\n"); 
        generate_set_num($3, $4); 
       }
       | LPAREN SET ID ID RPAREN { 
        //printf("DEBUG: Processing SETSTMT: set ID ID\n"); 
        generate_set_id($3, $4); 
       }
       | LPAREN SET ID EXPR RPAREN { 
        //printf("DEBUG: Processing SETSTMT: set ID EXPR (enter)\n");
        generate_set_expr($3, $4); 
        //printf("DEBUG: Processing SETSTMT: set ID EXPR (exit)\n");
       }
       ;

EXPRSTMT: EXPR { 
        //printf("DEBUG: Processing EXPRSTMT\n"); 
        generate_expr_stmt($1); 
        }
        ;

EXPR: LPAREN OP ARG ARG RPAREN {
        //printf("DEBUG: Processing EXPR: (%s %s %s)\n", $2, $3, $4); 
        $$ = generate_op($2, $3, $4); 
        //printf("DEBUG: Finished processing EXPR: (%s %s %s)\n", $2, $3, $4);
        }
    ;

OP: PLUS { $$ = "+"; /*printf("DEBUG: Processing OP: PLUS\n");*/ }
   | MINUS { $$ = "-"; /*printf("DEBUG: Processing OP: MINUS\n");*/ }
   | TIMES { $$ = "*"; /*printf("DEBUG: Processing OP: TIMES\n"); */}
   | DIVIDE { $$ = "/"; /*printf("DEBUG: Processing OP: DIVIDE\n");*/ }
   | MOD { $$ = "%"; /*printf("DEBUG: Processing OP: MOD\n"); */}
   | POWER { $$ = "**"; /*printf("DEBUG: Processing OP: POWER\n"); */}
   ;

ARG: ID { 
        $$ = format_string(0, get_var_offset($1)); 
       // printf("DEBUG: Processing ARG: ID (%s)\n", $1); 
        }
   | NUM { 
        $$ = format_string(2,atoi($1)); 
        //printf("DEBUG: Processing ARG: NUM (%s)\n", $1); 
        }
   | EXPR { 
        $$ = $1; 
        //printf("DEBUG: Processing ARG: EXPR\n"); 
        //printf("Line no %d\n", yylineno);   
        }
   ;

%%

void yyerror(const char* s) {
    printf("Parse error: at line no %d %s %s\n",yylineno,yytext, s);
    exit(1);
}
