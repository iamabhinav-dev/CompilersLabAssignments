#ifndef POLY_H
#define POLY_H

typedef struct node_t {
    struct node_t *child;
    struct node_t *sibling;
    char *inh;
    char *val;
    char type;
} Treenode;

//extern Treenode* headOfProduction;
extern int yyparse();
extern int yylex();
extern int yylineno;
extern char* yytext;
extern Treenode* headOfProduction;


void addChild(Treenode *parent, Treenode *child);
Treenode *createNode(char type);
#endif // POLY_H

