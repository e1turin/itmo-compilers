#pragma once

#ifndef _AST_H_
#define _AST_H_

#include <stdbool.h>
#include <stdint.h>

struct AST;

bool ast_alloc(struct AST** ptr);
void ast_free(struct AST* ast);

enum EXPR_OP {
  EXPR_ADD,
  EXPR_SUB,
  EXPR_MUL,
  EXPR_DIV,
  EXPR_AND,
  EXPR_OR,
  EXPR_ASSIGN,
  EXPR_NEG,
  EXPR_LESS,
  EXPR_EQ,
  EXPR_REM,
  EXPR_NOT
};

struct AST* make_ast_expr(enum EXPR_OP op, struct AST* lhs, struct AST* rhs);

struct AST* make_ast_code_block(struct AST* stmt, struct AST* code_block);

struct AST* make_ast_if_stmt(struct AST* cond, struct AST* then_part, struct AST* else_part);
struct AST* make_ast_if_chain(struct AST* cond, struct AST* then_part, struct AST* if_stmt);

struct AST* make_ast_while_stmt(struct AST* cond, struct AST* body);

struct AST* make_ast_print_stmt(struct AST* expr);

struct AST* make_ast_lit(int val);
struct AST* make_ast_var(char* name);

enum AST_NODE_TYPE { AST_EXPR, AST_LIT, AST_VAR, AST_IF, AST_WHILE, AST_CODE_BLOCK, AST_PRINT };

struct AST {
  enum AST_NODE_TYPE type;
  void* args[3];
};

#endif /* _AST_H_ */