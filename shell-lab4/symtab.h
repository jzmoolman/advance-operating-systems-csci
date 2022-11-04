extern int yyparse();

/* interface to the lexer */
extern int yylineno;   /* from lexer */
void yyerror(char*, ...);
 
struct stringrec {
    int nodetype;
    char* str;
};

struct stringrec *newstringrec(char*, int);
