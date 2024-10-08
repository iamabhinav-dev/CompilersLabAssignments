%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "poly.h"
    int yylex();
    void yyerror(const char *s);
    extern Treenode* headOfProduction;
%}

%code requires{
     #include "poly.h"
}

%union{
    char strVal;
    Treenode *node;
}
%token<strVal> D 
%token x RTP PLUS MINUS ZERO ONE

%type<node> S P T X N M

%start S


%%
S:
    P            {
        $$=createNode('S');
        addChild($$,$1);
        
        headOfProduction=$$;
        }
  | PLUS P        { 
        $$=createNode('S'); 
        addChild($$,createNode('+'));
        addChild($$,$2); 

        headOfProduction=$$;

      }
  | MINUS P        {
        $$=createNode('S'); 
        addChild($$,createNode('-'));
        addChild($$,$2); 

        headOfProduction=$$;

    }
  ;

P:
    T            { 
        $$=createNode('P');
        addChild($$,$1);
    }
  | T PLUS P      { 
        $$=createNode('P');
        addChild($$,$1);
        addChild($$,createNode('+'));
        addChild($$,$3);
   }
  | T MINUS P      { 
        $$=createNode('P');
        addChild($$,$1);
        addChild($$,createNode('-'));
        addChild($$,$3);
   }
  ;

T:
    ONE          {
        $$=createNode('T'); 
        addChild($$,createNode('1'));
         }
  | N            {
        $$=createNode('T'); 
        addChild($$,$1);
    }
  | X            { 
        $$=createNode('T'); 
        addChild($$,$1);
   }
  | N X          { 
        $$=createNode('T'); 
        addChild($$,$1);
        addChild($$,$2);
   }
  ;

X:
    x          { 
        $$=createNode('X'); 
        addChild($$,createNode('x'));
     }
  | x RTP N    { 
        $$=createNode('X'); 
        addChild($$,createNode('x'));
        addChild($$,createNode('^'));
        addChild($$,$3);
         }
  ;

N:
    D            { 
        $$=createNode('N'); 
        addChild($$,createNode($1));
     }
  | ONE M        { 
        $$=createNode('N'); 
        addChild($$,createNode('1'));
        addChild($$,$2);
   }
  | D M          { 
        $$=createNode('N'); 
        addChild($$,createNode($1));
        addChild($$,$2);
   }
  ;

M:
    ZERO          { 
        $$=createNode('M'); 
        addChild($$,createNode('0'));
     }
  | ONE          { 
        $$=createNode('M'); 
        addChild($$,createNode('1'));
   }
  | D            { 
        $$=createNode('M'); 
        addChild($$,createNode($1));
   }
  | ZERO M        { 
        $$=createNode('M'); 
        addChild($$,createNode('0'));
        addChild($$,$2);
   }
  | ONE M        { 
        $$=createNode('M'); 
        addChild($$,createNode('1'));
        addChild($$,$2);
   }
  | D M          { 
        $$=createNode('M'); 
        addChild($$,createNode($1));
        addChild($$,$2);

   }
  ;


%%

void yyerror(const char *s) {
    fprintf(stderr, "Parser error:at line no %d %s token was %s\n", yylineno,s,yytext);
}