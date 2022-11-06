#ifndef __SYMTAB__
#define __SYMTAB__

extern int yyparse();

/* interface to the lexer */
extern int yylineno;   /* from lexer */
void yyerror(char*, ...);

struct ast {
    int type;
    struct ast *l;
    struct ast *r;
    int bg;
};

struct symbol {
    char *name;
};

/* Simple fixed  */
#define NHASH 9997
struct symbol symtab[NHASH];
struct ast *root_ast;

struct symbol *lookup(char* sym);

struct param {
    struct symbol *sym;
    struct param *next;
};

struct ast *new_ast(int type, struct ast *l, struct ast *r, int bg);
struct param *new_param(struct symbol *sym);


#endif // __SYMTAB__


