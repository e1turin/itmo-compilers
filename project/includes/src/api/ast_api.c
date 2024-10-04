#include "ast_api.h"

#include <inttypes.h>
#include <stdint.h>

#include "ast.h"
#include "ast_walker.h"
#include "var_dict.h"

#include "lexer.h"
#include "parser.h"
#include "parser_res.h"

struct AST* ast_parse() {
  clear_dict();
  if (yyparse() != 0) {
    return (void*)0;
  }
  return get_parse_result();
}

struct AST* ast_parse_from_file(FILE* input) {
  clear_dict();
  YY_BUFFER_STATE state = yy_create_buffer(input, YY_BUF_SIZE);
  yy_switch_to_buffer(state);
  if (yyparse() != 0) {
    yy_delete_buffer(state);
    return (void*)0;
  }
  yy_delete_buffer(state);
  return get_parse_result();
}

static uint64_t next_id = 1;

void ast_walker(struct AST* ast, int from_id, FILE* out, int depth, printer_t printer) {
  uint64_t id = next_id++;

  if (!printer(ast, out, id, from_id, depth)) {
    return;
  }

  switch (ast->type) {
    case AST_WHILE:
    case AST_CODE_BLOCK:
      ast_walker(ast->args[0], id, out, depth + 1, printer);
      ast_walker(ast->args[1], id, out, depth + 1, printer);
      break;
    case AST_EXPR:
      ast_walker(ast->args[1], id, out, depth + 1, printer);
      ast_walker(ast->args[2], id, out, depth + 1, printer);
      break;
    case AST_IF:
      ast_walker(ast->args[0], id, out, depth + 1, printer);
      ast_walker(ast->args[1], id, out, depth + 1, printer);
      ast_walker(ast->args[2], id, out, depth + 1, printer);
    default:
      break;
  }
}
