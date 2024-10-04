#include "ast_api.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
  struct AST* ast;
  if (argc == 1) {
    ast = ast_parse();
  } else if (argc == 2) {
    FILE* file = fopen(argv[1], "r");
    if (!file) {
      fprintf(stderr, "Unable to open file %s", argv[1]);
      ast = NULL;
    } else {
      ast = ast_parse_from_file(file);
      fclose(file);
    }
  } else {
    ast = NULL;
  }

  FILE* file = fopen("graph.dot", "w+");
  if (file) {
    ast_graphviz(ast, file);
    fclose(file);
  } else {
    fprintf(stderr, "Unable to open file graph.dot");
  }

  ast_print(ast, stdout);

  ast_print_riscv(ast, "riscv.asm");

  ast_free(ast);
  return 0;
}