#include <stdio.h>
#include <stdlib.h>  // malloc
#include <string.h>  // memset
#include <stdarg.h> // valist
#include "symtab.h"

void yyerror(char *s, ...) {
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

static unsigned symhash(char *sym) {
  unsigned int hash = 0;
  unsigned c;

  while((c = *sym++)) hash = hash*9 ^ c;

  return hash;
}

struct symbol *lookup(char* sym) {
  struct symbol *sp = &symtab[symhash(sym)%NHASH];
  int scount = NHASH;		/* how many have we looked at */

  while(--scount >= 0) {
    if(sp->name && !strcmp(sp->name, sym)) { return sp; }

    if(!sp->name) {		/* new entry */
      sp->name = strdup(sym);
      return sp;
    }

    if(++sp >= symtab+NHASH) sp = symtab; /* try the next entry */
  }
  yyerror("symbol table overflow\n");
  abort(); /* tried them all, table is full */
}

struct ast *new_ast(int type, struct ast *l, struct ast *r, int bg) {
    struct ast *_ast = malloc(sizeof(struct ast));
    if (!_ast){
        yyerror("out of space");
        exit(0);
    }
    memset(_ast, 0, sizeof(struct ast));
    _ast->type = type;
    _ast->l = l;
    _ast->r = r;
    _ast->bg = bg;
    return _ast;

}


struct param *new_param(struct symbol *sym) {
    struct param *param = malloc(sizeof(struct param));
    if (!param){
        yyerror("out of space");
        exit(0);
    }
    memset(param, 0, sizeof(struct param));

    param->sym = sym;
    return param;

};