%{
#include <stdio.h>
#include <stdint.h>

#include "ast.h"
#include "parser_res.h"

int yylex (void);
void yyerror (char const * err) {
  fprintf(stderr, "Parse Error: %s\n", err);
}
%}
/* Bison declarations. */
%define parse.error verbose

%union {
  char* as_string;
  int64_t as_int;
  struct AST* as_ast;
}

%token IF
%token ELSE

%token WHILE

%token PRINT

%token <as_string> VAR_LIT
%token <as_int> INT_LIT
%token <as_int> BOOL_LIT

%right '='
%left '&' '|'
%left '<' EQ
%left '+' '-'
%left '*' '/' '%'
%precedence UMINUS NOT

%destructor {
  ast_free($$);
} <as_ast>

%destructor {
  free($$);
} <as_string>

%type <as_ast> expr lit stmt code_block while_stmt if_stmt io_stmt

%%
program: code_block { set_parse_result($1); YYACCEPT; }

code_block: stmt { $$ = make_ast_code_block($1, NULL); }
| code_block stmt { $$ = make_ast_code_block($2, $1); }

stmt: expr ';' { $$ = $1; }
| if_stmt { $$ = $1; }
| while_stmt { $$ = $1; }
| io_stmt { $$ = $1; }

if_stmt: IF '(' expr ')' '{' code_block '}' { $$ = make_ast_if_stmt($3, $6, NULL); }
| IF '(' expr ')' '{' code_block '}' ELSE '{' code_block '}' { $$ = make_ast_if_stmt($3, $6, $10); }
| IF '(' expr ')' '{' code_block '}' ELSE if_stmt { $$ = make_ast_if_chain($3, $6, $9); }

while_stmt: WHILE '(' expr ')' '{' code_block '}' { $$ = make_ast_while_stmt($3, $6); }

io_stmt: PRINT '(' expr ')' ';' { $$ = make_ast_print_stmt($3); }

expr: expr '+' expr         { $$ = make_ast_expr(EXPR_ADD, $1,$3); }
   | expr '-' expr          { $$ = make_ast_expr(EXPR_SUB, $1,$3); }
   | expr '*' expr          { $$ = make_ast_expr(EXPR_MUL, $1,$3); }
   | expr '/' expr          { $$ = make_ast_expr(EXPR_DIV, $1,$3); }
   | expr '%' expr          { $$ = make_ast_expr(EXPR_REM, $1,$3); }
   | expr '&' expr          { $$ = make_ast_expr(EXPR_AND, $1,$3); }
   | expr '|' expr          { $$ = make_ast_expr(EXPR_OR, $1,$3); }
   | expr '=' expr       { $$ = make_ast_expr(EXPR_ASSIGN, $1,$3); }
   | expr '<' expr          { $$ = make_ast_expr(EXPR_LESS, $1,$3);}
   | expr EQ expr           { $$ = make_ast_expr(EXPR_EQ, $1,$3);}
   | '(' expr ')'           { $$ = $2; }
   | '!' expr %prec NOT     { $$ = make_ast_expr(EXPR_NOT, $2, NULL);}
   | '-' expr %prec UMINUS  { $$ = make_ast_expr(EXPR_NEG, $2, NULL); }
   | lit                    { $$ = $1; }

lit: VAR_LIT    { $$ = make_ast_var($1); }
| INT_LIT       { $$ = make_ast_lit($1); }
| BOOL_LIT      { $$ = make_ast_lit($1); }
   
%%
