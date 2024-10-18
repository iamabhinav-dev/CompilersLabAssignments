#ifndef INTCODEGEN_H
#define INTCODEGEN_H

extern int yyparse();
extern int yylex();
extern int yylineno;;
extern char* yytext;


char *generate_set_num(char* id, char* num);
char *generate_set_id(char* id1, char* id2);
char  *generate_set_expr(char *id, char* expr_reg);
void generate_expr_stmt(char *expr_reg);
char *generate_op(char *op, char *arg1, char *arg2);
char *format_string(int type, int value);
int get_var_offset(char *id);
void freeReg(char * expr);
#endif