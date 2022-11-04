/* yazh - yet another zach shell  */

%option noyywrap nodefault yylineno
%{
#include <stdio.h>
#include <string.h>
#include "symtab.h"
#include "yazh.tab.h"

int line_number = 0;

%}

%x COMMENT1 COMMENT2

white_space       [ \t]*
digit             [0-9]
alpha             [A-Za-z_]
alpha_num         ({alpha}|{digit})
hex_digit         [0-9A-F]
command           {alpha}{alpha_num}*
unsigned_integer  {digit}+
hex_integer       ${hex_digit}{hex_digit}*
exponent          e[+-]?{digit}+
i                 {unsigned_integer}
real              ({i}\.{i}?|{i}?\.{i}){exponent}?
var               \${command}
string            \'([^'\n]|\'\')+\'
bad_string        \'([^'\n]|\'\')+
%%

{var}               {  
                        struct stringrec *a = newstringrec(yytext,'V');
                        yylval.v = a;
                        return VAR;
                    }
{string}            {  
                        struct stringrec *a = newstringrec(yytext, 'S');
                        yylval.s = a;
                        return STRING;
                    }
{command}           {  
                        struct stringrec *a = newstringrec(yytext, 'C');
                        yylval.c = a;
                        return COMMAND;
                    }
{bad_string}         yyerror("Unterminated string");
{white_space}        /* do nothing */
\n                   line_number += 1;
.                    yyerror("Illegal input");

%%

