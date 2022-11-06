%{
    #include <stdio.h>
    #include "symtab.h"
    #include "posixfunc.h"
    extern int yylex(void);
%}

%union {
  struct symbol *s;
  struct param *pl;
  struct ast *a;
}

/* declare tokens */
%token <int> PWD CD ECHO  '&'
%token <s> WORD VAR STRING SET
%token EOL
%type <s> command
%type <pl> param word

%%
input:
|    line    
|    input line

line:
    EOL
|   PWD EOL             { root_ast = new_ast(PWD,NULL,NULL,0);                      } 
|   CD WORD EOL         { root_ast = new_ast(CD,(struct ast *)$2,NULL,0);            }
|   SET EOL             { root_ast = new_ast(SET,(struct ast *)$1,NULL,0);  }
|   ECHO param EOL      {  
                            int count = 0; 
                            for ( struct param *t = $2; t!= NULL; t = t->next)
                                count++;

                            char** param = malloc(sizeof(char*)*count+1);
                            char** p = param;
                            for ( struct param *t = $2; t!= NULL; t = t->next) {
                                *p = t->sym->name;
                                p++;
                            }
                            *p = NULL; // should this go here or in eval??

                            root_ast = new_ast(ECHO, (struct ast*)param, NULL,0);
                        }
|   command param EOL   {                                   // printf("command: param %s\n", $1->name);
                            char *fp = find_cmd($1->name);
                                                            // printf("D0\n");
                                                            // printf("%s\n", fp);
                            if (fp) {

                                int count = 0; 
                                for ( struct param *t = $2; t!= NULL; t = t->next)
                                    count++;
                                
                                char** param = malloc(sizeof(char*)*count+2); // 1 for filename 1 for \0
                                char** p = param;
                                *p = $1->name; 
                                p++;
                                for ( struct param *t = $2; t!= NULL; t = t->next) {
                                    *p = t->sym->name;
                                    p++;
                                }
                                *p = NULL;

                                exe_cmd(fp, count+1, param,0);

                             } else {
                                printf("file not found\n");
                             }
                             printf("> ");           
                        } 
|   command param '&' EOL   {                                   // printf("command: param %s\n", $1->name);
                            char *fp = find_cmd($1->name);
                                                            // printf("D0\n");
                                                            // printf("%s\n", fp);
                            if (fp) {

                                int count = 0; 
                                for ( struct param *t = $2; t!= NULL; t = t->next)
                                    count++;
                                
                                char** param = malloc(sizeof(char*)*count+2); // 1 for filename 1 for \0
                                char** p = param;
                                *p = $1->name; 
                                p++;
                                for ( struct param *t = $2; t!= NULL; t = t->next) {
                                    *p = t->sym->name;
                                    p++;
                                }
                                *p = NULL;

                                exe_cmd(fp, count+1, param, 1);

                             } else {
                                printf("file not found\n");
                             }
                             printf("> ");           
                        } 
|   command EOL   {                                         // printf("command: EOL%s\n", $1->name);
                            char *fp = find_cmd($1->name);
                            if (fp) {

                                int count = 0; 
                                char** param = malloc(sizeof(char*)*count+2); // 1 for filename 1 for \0
                                *param = $1->name;
                                *(param+1) = NULL;
                                exe_cmd(fp, count+1, param, 0);

                             } else {
                                printf("file not found\n");
                             }
                             printf("> ");           
                        } 
|   command  '&' EOL   {                                          // printf("command: EOL &%s\n", $1->name);
                            char *fp = find_cmd($1->name);
                            if (fp) {

                                int count = 0; 
                                char** param = malloc(sizeof(char*)*count+2); // 1 for filename 1 for \0
                                *param = $1->name;
                                *(param+1) = NULL;
                                exe_cmd(fp, count+1, param, 1);

                             } else {
                                printf("file not found\n");
                             }
                             printf("> ");           
                        } 
command: WORD           {   
                            $$ = $1; 
                        } 

param: word             {    $$ = $1; } 
|   word   param        {   $1->next = $2; $$ = $1; }

word: WORD              {   
                            struct param *a = new_param($1);
                            $$ = a;
                        }
|    VAR                {   char* var = get_env_var($1->name);
                            struct param *b;
                            if ( var != NULL ) {
                                b = new_param(lookup(var));
                            } else {
                                b = new_param(lookup("{EMPTY}"));
                            }
                            $$ = b;
                        }


%%
