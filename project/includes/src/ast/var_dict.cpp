#include "var_dict.h"

#include <bits/stdc++.h>

struct var_dict_entry {
  std::string name;
  int cnt;
  int addr;
};

static std::map<std::string, std::unique_ptr<var_dict_entry>> dict;
static int next_addr = VAR_START;

struct var_dict_entry* get_variable(char* name) {
  auto str = std::string(name);

  auto [it, res] = dict.try_emplace(str, std::make_unique<var_dict_entry>(var_dict_entry{str, 0, next_addr++}));
  it->second->cnt++;
  return it->second.get();
}

int get_variable_addr(struct var_dict_entry* var) {
  return var->addr;
}
const char* get_variable_name(struct var_dict_entry* var) {
  return var->name.c_str();
}

void free_variable(struct var_dict_entry* var) {
  auto it = dict.find(var->name);
  if (--it->second->cnt == 0) {
    dict.erase(it);
  }
}

void clear_dict() {
  dict.clear();
}