#include "ast_api.h"

#include <inttypes.h>
#include <stdint.h>

#include "ast.h"
#include "ast_walker.h"
#include "var_dict.h"

static char* EXPR_OP_NAME[] = {
    [EXPR_ADD] = "+",    [EXPR_SUB] = "-", [EXPR_MUL] = "*",  [EXPR_DIV] = "/", [EXPR_AND] = "&",  [EXPR_OR] = "|",
    [EXPR_ASSIGN] = "=", [EXPR_NEG] = "-", [EXPR_LESS] = "<", [EXPR_EQ] = "==", [EXPR_REM] = "\%", [EXPR_NOT] = "!"};

static char* AST_NODE_NAME[] = {
    [AST_EXPR] = "AST_EXPR",  [AST_LIT] = "AST_LIT",     [AST_VAR] = "AST_VAR",
    [AST_IF] = "AST_IF",      [AST_WHILE] = "AST_WHILE", [AST_CODE_BLOCK] = "AST_CODE_BLOCK",
    [AST_PRINT] = "AST_PRINT"};

static void ast_print_node_name(struct AST* ast, FILE* out) {
  fprintf(out, "%s", AST_NODE_NAME[ast->type]);
  switch (ast->type) {
    case AST_LIT:
      fprintf(out, "=%d", *((int*)ast->args[0]));
      break;
    case AST_VAR:
      fprintf(out, "=%s %d", get_variable_name(ast->args[0]), get_variable_addr(ast->args[0]));
      break;
    case AST_EXPR:
      fprintf(out, " %s", EXPR_OP_NAME[*((enum EXPR_OP*)ast->args[0])]);
      break;
    default:
      break;
  }
}

static void ast_print_offset(FILE* out, int depth) {
  for (int i = 0; i < depth - 1; i++) {
    fprintf(out, "|  ");
  }
  if (depth > 0) {
    fprintf(out, "|--");
  }
}

bool ast_base_printer(struct AST* ast, FILE* out, uint64_t id, uint64_t from_id, uint64_t depth) {
  ast_print_offset(out, depth);
  fprintf(out, "+-");

  if (!ast) {
    fprintf(out, "<NULL>\n");
    return false;
  }

  ast_print_node_name(ast, out);

  fprintf(out, "\n");
  return true;
}

void ast_print(struct AST* ast, FILE* out) {
  ast_walker(ast, 0, out, 0, &ast_base_printer);
}

bool ast_graphviz_printer(struct AST* ast, FILE* out, uint64_t id, uint64_t from_id, uint64_t depth) {
  if (!ast) {
    return false;
  }
  fprintf(out, "%" PRId64 " [label = \"", id);
  ast_print_node_name(ast, out);
  fprintf(out, "\";];\n");

  if (from_id != 0) {
    fprintf(out, "%" PRId64 " -> %" PRId64 ";\n", from_id, id);
  }
  return true;
}

void ast_graphviz(struct AST* ast, FILE* out) {
  fprintf(out, "digraph {\n");
  ast_walker(ast, 0, out, 0, &ast_graphviz_printer);
  fprintf(out, "}\n");
}