#include "parser_res.h"

#include "ast_api.h"

struct AST* parse_res = NULL;

void set_parse_result(struct AST* res) {
  parse_res = res;
}
struct AST* get_parse_result() {
  struct AST* res = parse_res;
  parse_res = (void*)0;
  return res;
}