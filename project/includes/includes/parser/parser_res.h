#pragma once

#ifndef _PARSER_RES_H_
#define _PARSER_RES_H_

struct AST;

void set_parse_result(struct AST* res);
struct AST* get_parse_result();

#endif /* _PARSER_RES_H_ */