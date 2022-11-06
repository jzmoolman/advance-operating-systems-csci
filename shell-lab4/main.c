#include <stdio.h>
#include "symtab.h"
#include  "yazh.tab.h"
#include "posixfunc.h"

// typedef struct yy_buffer_state *YY_BUFFER_STATE
extern struct yy_buffer_state *yy_scan_string(char*);
extern struct yy_buffer_state *yy_scan_buffer(char*, size_t);
extern void yy_delete_buffer(struct yy_buffer_state *buffer);
extern void yy_switch_to_buffer(struct yy_buffer_state *buffer);

void print_prompt(char* s){
    printf("%s", s);
    fflush(stdout);
}

char* read_line() {
    char* line = NULL;
    size_t len = 0;
    getline(&line, &len, stdin);
    return line;
}

int eval_ast(struct ast *a) {
    if (a == NULL) {
        return 0;
    }
    switch (a->type)
    {
    case PWD: cmd_pwd(); break;
    case CD: cmd_cd(((struct symbol *)a->l)->name); break;
    case ECHO: cmd_echo((char**)(a->l)); break;
    case SET: cmd_set(((struct symbol *)a->l)->name); break;
    default:
        break;
    }
    return 0;
}

int parse_line(char* line) {
    struct ast *_ast;
    struct yy_buffer_state *buffer = yy_scan_string(line);
    yy_switch_to_buffer(buffer);
    yyparse(); // check any errors?
    eval_ast(root_ast);
    root_ast = NULL;
    yy_delete_buffer(buffer);
    return 0; //  retruns 0 when success otherwise report back process result
}

int main() {
    // return yyparse();
    while (1) {
        print_prompt("> ");
        parse_line(read_line());
    }
    return 0;
}

