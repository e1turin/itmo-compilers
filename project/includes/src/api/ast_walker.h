#pragma once

typedef bool (*printer_t)(struct AST*, FILE*, uint64_t, uint64_t, uint64_t);

void ast_walker(struct AST* ast, int from_id, FILE* out, int depth, printer_t printer);