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
alpha             [-A-Za-z_/]
alpha_num         ({alpha}|{digit})
hex_digit         [0-9A-F]
word              {alpha_num}*
unsigned_integer  {digit}+
hex_integer       ${hex_digit}{hex_digit}*
exponent          e[+-]?{digit}+
i                 {unsigned_integer}
real              ({i}\.{i}?|{i}?\.{i}){exponent}?
var               \${word}
string            \'([^'\n]|\'\')+\'
bad_string        \'([^'\n]|\'\')+
%%
pwd                 {  return PWD; }

cd                  {  return CD; }
echo                {  return ECHO2; }

{var}               {  
                        struct symbol *a = lookup(yytext);
                        yylval.s = a;
                        return VAR;
                    }
{string}            {  
                        struct symbol *a = lookup(yytext);
                        yylval.s = a;
                        return STRING;
                    }
{word}              {   struct symbol *a = lookup(yytext);
                        yylval.s = a;
                        return WORD;
                    }
{bad_string}         yyerror("Unterminated string");
{white_space}        /* do nothing */
&                   return '&';
\n                  {
                        line_number += 1;
                        return EOL;
                    }
{word}={word}       { 
                        struct symbol *a = lookup(yytext);
                        yylval.s = a;
                        return SET;
                    }
.                    yyerror("Illegal input");

%%

