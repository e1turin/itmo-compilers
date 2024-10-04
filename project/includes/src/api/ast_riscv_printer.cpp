#include "ast_api.h"

#include "ast.h"
#include "ast_walker.h"
#include "var_dict.h"

#include <bits/stdc++.h>
#include <cstdio>

#define EXPR_RES_REG 31
#define FREE_ADDR_START 0x300

using var_deps_t = std::map<std::string, std::pair<int, var_dict_entry*>>;

static void discover_deps(var_deps_t& var_deps, AST* ast, int& next_reg) {
  if (ast == nullptr) {
    return;
  }

  auto dict_entry = reinterpret_cast<var_dict_entry*>(ast->args[0]);

  if (ast->type == AST_VAR) {
    auto [it, emplaced] = var_deps.try_emplace(get_variable_name(dict_entry), std::make_pair(0, dict_entry));
    if (emplaced) {
      it->second.first = next_reg--;
      if (next_reg == -1) {
        throw std::runtime_error("Too many variables in expression, max 29");
      }
    }
  } else if (ast->type == AST_EXPR) {
    discover_deps(var_deps, static_cast<AST*>(ast->args[1]), next_reg);
    discover_deps(var_deps, static_cast<AST*>(ast->args[2]), next_reg);
  }
}

static std::string expr_op_name[] = {
    [EXPR_AND] = "and", [EXPR_OR] = "or",   [EXPR_DIV] = "div",  [EXPR_REM] = "rem", [EXPR_ADD] = "add",
    [EXPR_SUB] = "sub", [EXPR_MUL] = "mul", [EXPR_LESS] = "slt", [EXPR_EQ] = "seq",
};

static int ast_riscv_op(AST* ast, std::ostream& out, var_deps_t& var_deps, int lhs, int rhs, int free_reg,
                        int free_addr) {
  char cmd_buf[256];
  if (ast->type == AST_EXPR) {
    auto op = *(reinterpret_cast<EXPR_OP*>(ast->args[0]));

    if (op == EXPR_ASSIGN) {
      int cmd_cnt = ast_riscv_op(reinterpret_cast<AST*>(ast->args[2]), out, var_deps, lhs, rhs, free_reg, free_addr);
      auto l_arg = reinterpret_cast<AST*>(ast->args[1]);
      if (l_arg->type != AST_VAR) {
        throw std::runtime_error("Expected variable");
      }
      auto dict_entry = reinterpret_cast<var_dict_entry*>(l_arg->args[0]);
      sprintf(cmd_buf, "sw x0, %d, x%d\n", get_variable_addr(dict_entry), lhs);
      out << cmd_buf;
      sprintf(cmd_buf, "add x%d, x0, x%d\n", var_deps[get_variable_name(dict_entry)].first, lhs);
      out << cmd_buf;
      return cmd_cnt + 2;
    }

    int cmd_cnt = ast_riscv_op(reinterpret_cast<AST*>(ast->args[1]), out, var_deps, lhs, rhs, free_reg, free_addr);

    if (op == EXPR_NOT) {
      sprintf(cmd_buf, "li x%d, 0\n", rhs);
      out << cmd_buf;
      sprintf(cmd_buf, "seq x%d, x%d, x%d\n", lhs, lhs, rhs);
      out << cmd_buf;
      return cmd_cnt + 2;
    }

    if (op == EXPR_NEG) {
      sprintf(cmd_buf, "li x%d, -1\n", rhs);
      out << cmd_buf;
      sprintf(cmd_buf, "mul x%d, x%d, x%d", lhs, rhs, lhs);
      out << cmd_buf;
      return cmd_cnt + 2;
    }

    auto stored = false;
    if (free_reg == 0) {
      sprintf(cmd_buf, "sw x0, x%d, x%d\n", free_addr, lhs);
      out << cmd_buf;
      free_addr++;
      stored = true;
    } else {
      sprintf(cmd_buf, "add x%d, x0, x%d\n", free_reg, lhs);
      out << cmd_buf;
      free_reg--;
    }
    cmd_cnt++;

    cmd_cnt += ast_riscv_op(reinterpret_cast<AST*>(ast->args[2]), out, var_deps, rhs, lhs, free_reg, free_addr);

    if (stored) {
      free_addr--;
      sprintf(cmd_buf, "lw x%d, x0, %d\n", lhs, free_addr);
      out << cmd_buf;
    } else {
      free_reg++;
      sprintf(cmd_buf, "add x%d, x0, x%d\n", lhs, free_reg);
      out << cmd_buf;
    }
    cmd_cnt += 1;

    sprintf(cmd_buf, "%s x%d, x%d, x%d\n", expr_op_name[op].c_str(), lhs, lhs, rhs);
    out << cmd_buf;
    cmd_cnt += 1;
    return cmd_cnt;
  } else if (ast->type == AST_LIT) {
    sprintf(cmd_buf, "li x%d, %d\n", lhs, *(reinterpret_cast<int*>(ast->args[0])));
    out << cmd_buf;
    return 1;
  } else if (ast->type == AST_VAR) {
    auto reg = var_deps[get_variable_name(reinterpret_cast<var_dict_entry*>(ast->args[0]))].first;
    sprintf(cmd_buf, "add x%d, x%d, x0\n", lhs, reg);
    out << cmd_buf;
    return 1;
  } else {
    throw std::runtime_error("SHOULD NOT HAPPEN");
  }
}

static int ast_riscv_expr(AST* ast, std::ostream& out, int res) {
  var_deps_t var_deps;
  int next_reg = 28;
  discover_deps(var_deps, ast, next_reg);

  for (auto& p : var_deps) {
    char cmd_buf[255];
    sprintf(cmd_buf, "lw x%d, x0, %d\n", p.second.first, get_variable_addr(p.second.second));
    out << cmd_buf;
  }

  return ast_riscv_op(ast, out, var_deps, res, res - 1, res - 2, FREE_ADDR_START) + var_deps.size();
}

static int ast_riscv(AST* ast, std::ostream& out);

static int ast_riscv_if(AST* ast, std::ostream& out) {
  out << "# IF START\n";
  std::stringstream ss;
  int then_cnt = ast_riscv(reinterpret_cast<AST*>(ast->args[1]), ss) + 1;

  int res = ast_riscv_expr(reinterpret_cast<AST*>(ast->args[0]), out, EXPR_RES_REG);

  char cmd_buf[255];
  sprintf(cmd_buf, "li x%d, 0\n", EXPR_RES_REG - 1);
  out << cmd_buf;
  sprintf(cmd_buf, "beq x%d, x%d, %d\n", EXPR_RES_REG, EXPR_RES_REG - 1, then_cnt);
  out << cmd_buf;
  res += 2;

  std::stringstream else_ss;
  int else_cnt = ast_riscv(reinterpret_cast<AST*>(ast->args[2]), else_ss);

  out << "# THEN START\n";
  out << ss.str();

  sprintf(cmd_buf, "jal x0, %d\n", else_cnt);
  out << cmd_buf;
  res += then_cnt;

  out << "# ELSE START\n";
  out << else_ss.str();
  res += else_cnt;

  out << "# ELSE FINISH\n";
  return res;
}

static int ast_riscv_print(AST* ast, std::ostream& out) {
  int res = ast_riscv_expr(reinterpret_cast<AST*>(ast->args[0]), out, EXPR_RES_REG);

  out << "ewrite x" << EXPR_RES_REG << std::endl;

  return res + 1;
}

static int ast_riscv_while(AST* ast, std::ostream& out) {
  std::stringstream cond_ss;
  int cond_cnt = ast_riscv_expr(reinterpret_cast<AST*>(ast->args[0]), cond_ss, EXPR_RES_REG);

  std::stringstream body_ss;
  int body_cnt = ast_riscv(reinterpret_cast<AST*>(ast->args[1]), body_ss);
  body_cnt += 1;

  cond_cnt += 2;
  out << "# COND START " << cond_cnt << "\n";
  out << cond_ss.str();
  out << "li x" << EXPR_RES_REG - 1 << ", 0\n";
  out << "beq x" << EXPR_RES_REG << ", x" << EXPR_RES_REG - 1 << ", " << body_cnt << "\n";

  out << "# BODY START " << body_cnt << "\n";
  out << body_ss.str();
  out << "jal x0, " << -body_cnt - cond_cnt << "\n";
  out << "# BODY FINISH\n";

  return cond_cnt + body_cnt;
}

static int ast_riscv(AST* ast, std::ostream& out) {
  if (!ast) {
    return 0;
  }

  int res;

  switch (ast->type) {
    case AST_CODE_BLOCK:
      res = ast_riscv(reinterpret_cast<AST*>(ast->args[1]), out);
      res += ast_riscv(reinterpret_cast<AST*>(ast->args[0]), out);
      break;
    case AST_EXPR:
      res = ast_riscv_expr(ast, out, EXPR_RES_REG);
      break;
    case AST_IF:
      res = ast_riscv_if(ast, out);
      break;
    case AST_PRINT:
      res = ast_riscv_print(ast, out);
      break;
    case AST_WHILE:
      res = ast_riscv_while(ast, out);
      break;
    default: {
      res = 0;
      break;
    }
  }

  return res;
}

void ast_print_riscv(struct AST* ast, char* filename) {
  std::ofstream out(filename, std::ios_base::out);
  ast_riscv(ast, out);
  out << "ebreak\n";
}