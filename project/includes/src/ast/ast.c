#include "ast.h"

#include "var_dict.h"

#include <stdio.h>
#include <stdlib.h>

bool ast_alloc(struct AST** ptr) {
  *ptr = calloc(1, sizeof(struct AST));
  return (*ptr != NULL);
}

void ast_free(struct AST* ptr) {
  if (ptr == NULL) {
    return;
  }

  switch (ptr->type) {
    case AST_EXPR:
      free(ptr->args[0]);
      ast_free(ptr->args[1]);
      ast_free(ptr->args[2]);
      break;
    case AST_IF:
      ast_free(ptr->args[0]);
      ast_free(ptr->args[1]);
      ast_free(ptr->args[2]);
      break;
    case AST_LIT:
      free(ptr->args[0]);
      break;
    case AST_VAR:
      free_variable(ptr->args[0]);
      break;
    case AST_CODE_BLOCK:
    case AST_WHILE:
      ast_free(ptr->args[0]);
      ast_free(ptr->args[1]);
      break;
    case AST_PRINT:
      ast_free(ptr->args[0]);
      break;
    default:
      break;
  }
  free(ptr);
}

struct AST* make_ast_expr(enum EXPR_OP op, struct AST* lhs, struct AST* rhs) {
  fprintf(stderr, "MAKE EXPR %d\n", op);
  struct AST* res;
  ast_alloc(&res);
  res->type = AST_EXPR;
  res->args[0] = malloc(sizeof(enum EXPR_OP));
  *((enum EXPR_OP*)res->args[0]) = op;
  res->args[1] = lhs;
  res->args[2] = rhs;
  return res;
}

struct AST* make_ast_code_block(struct AST* stmt, struct AST* code_block) {
  fprintf(stderr, "MAKE CODE BLOCK %p %p\n", stmt, code_block);
  struct AST* res;
  ast_alloc(&res);

  res->type = AST_CODE_BLOCK;

  if (code_block == NULL) {
    res->args[1] = stmt;
    return res;
  }

  code_block->args[0] = stmt;
  res->args[1] = code_block;
  return res;
}

struct AST* make_ast_if_stmt(struct AST* cond, struct AST* then_part, struct AST* else_part) {
  struct AST* res;
  ast_alloc(&res);
  res->type = AST_IF;
  res->args[0] = cond;
  res->args[1] = then_part;
  res->args[2] = else_part;
  return res;
}
struct AST* make_ast_if_chain(struct AST* cond, struct AST* then_part, struct AST* if_stmt) {
  struct AST* res;
  ast_alloc(&res);
  res->type = AST_IF;
  res->args[0] = cond;
  res->args[1] = then_part;
  res->args[2] = make_ast_code_block(if_stmt, NULL);
  return res;
}

struct AST* make_ast_while_stmt(struct AST* cond, struct AST* body) {
  struct AST* res;
  ast_alloc(&res);
  res->type = AST_WHILE;
  res->args[0] = cond;
  res->args[1] = body;
  return res;
}

struct AST* make_ast_lit(int val) {
  struct AST* res;
  ast_alloc(&res);
  res->type = AST_LIT;
  res->args[0] = malloc(sizeof(int));
  *((int*)res->args[0]) = val;
  return res;
}

struct AST* make_ast_var(char* name) {
  struct AST* res;
  ast_alloc(&res);
  res->type = AST_VAR;
  res->args[0] = get_variable(name);
  free(name);
  return res;
}

struct AST* make_ast_print_stmt(struct AST* expr) {
  struct AST* res;
  ast_alloc(&res);
  res->type = AST_PRINT;
  res->args[0] = expr;
  return res;
}