%option noyywrap

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "parser.h"
%}

VAR_LIT [a-zA-Z][a-zA-Z0-9]*
INT_LIT [0-9]+

%%

"==" {return EQ;}
"if" {return IF;}
"else" {return ELSE;}
"while" {return WHILE;}
"print" {return PRINT;}

"true" {
  yylval.as_int = 1;
  return BOOL_LIT;
}
"false" {
  yylval.as_int = 0;
  return BOOL_LIT;
}

{VAR_LIT} {
  int len = strlen(yytext);
  yylval.as_string = malloc(sizeof(char) * (len + 1));
  strncpy(yylval.as_string, yytext, len);
  yylval.as_string[len] = '\0';
  fprintf(stderr, "FOUND VAR_LIT: %s\n", yylval.as_string);
  return VAR_LIT;
}
{INT_LIT} {
  yylval.as_int = atoi(yytext);
  return INT_LIT;
}
[[:space:]] {}
. {return yytext[0];}

%%