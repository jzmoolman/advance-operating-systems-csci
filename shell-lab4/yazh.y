%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "symtab.h"
    extern int yylex(void);
%}

%union {
  struct stringrec *c;
  struct stringrec *s;
  struct stringrec *v;
}

/* declare tokens */
%token <c> COMMAND
%token <s> STRING
%token <v> VAR
%nterm line

%%

line: 
                        { printf("empty\n"); } 
|   line COMMAND     { printf("command %s\n", $2->str); }
|   line STRING      { printf("string %s\n", $2->str); }
|   line VAR         { printf("VAR %s\n", $2->str); }
;

%%
