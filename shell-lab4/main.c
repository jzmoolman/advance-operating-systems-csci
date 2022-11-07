#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strdup
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

char **param_to_args(struct param * p) {
    int count = 0; 
    for ( struct param *t = p; t!= NULL; t = t->next)
        count++;

    char** args = malloc(sizeof(char **) * (count+1) );
    char** c = args; 
    for ( struct param *t = p; t!= NULL; t = t->next) {
        *c = t->sym->name;
        c++;
    }
    *c = NULL; 
    return args;
}

char **param_to_args2(struct param * p) {
    int count = 0; 
    if (p != NULL)          
        for ( struct param *t = p; t!= NULL; t = t->next)
             count++;
                                    // printf("D0: count %d\n", count); 
   
    char** args = malloc(sizeof(char*)*count+2); // 1 for filename 1 for \0
    *args = strdup("{filename}"); // reserve the first entry to relative fielname
    char** c = args+1;
    if ( p!=NULL) { // If param is not NULL
        for ( struct param *t = p; t!= NULL; t = t->next) {
            *c = strdup(t->sym->name);
            c++;
        }
    } 
    *c = NULL; 
    return args;
}

void print_args(char **args) {
                                    // printf("d0: %p\n", args);
    for(;*args != NULL; args++)
        printf("%s\n", *args);
                                    // printf("d1: %p\n", args);
}

int eval_ast(struct ast *a) {
    if (a == NULL) {
        return 0;
    }
    int result = 0;
    switch (a->type)
    {
    case PWD: cmd_pwd(); break;
    case CD: cmd_cd(((struct symbol *)a->l)->name); break;
    case ECHO2: { 
        char **args = param_to_args( (struct param *) (a->l) );
        cmd_echo(args); 
        break;
    }
    case SET: cmd_set(((struct symbol *)a->l)->name); break;
    case CMD: { 
        char *filename = ((struct symbol *)a->l)->name;
        char *abs_filename = find_cmd(filename);
        if (abs_filename == NULL) {
            printf("yazh: command not found: %s\n", filename );
            break;
        } else {
            char **args = param_to_args2((struct param *)a->r);
            *args = strdup(filename);
                                                // print_args(args);
            if (a->bg == 0)
                result = exe_cmd(abs_filename, args, 0);
            else {
                result = exe_cmd(abs_filename, args, 1); 
            }
        }
        break;
    }
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

