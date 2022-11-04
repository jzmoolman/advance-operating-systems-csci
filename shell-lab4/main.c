

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "symtab.h"

struct stringrec *newstringrec(char* str, int nodetype ) {
    struct stringrec *a =  malloc(sizeof(struct stringrec));
	if (!a) {
        yyerror("Out of space\n");
        exit(0);
    }

    a->nodetype = nodetype;
    a->str = strdup(str); 
    return a;
}


void yyerror(char *s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

int main() {
    printf("> ");
    return yyparse();
}

