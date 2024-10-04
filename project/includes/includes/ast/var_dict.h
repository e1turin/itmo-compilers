#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define VAR_START 0x200

struct var_dict_entry;

struct var_dict_entry* get_variable(char* name);

int get_variable_addr(struct var_dict_entry* var);
const char* get_variable_name(struct var_dict_entry* var);

void free_variable(struct var_dict_entry* var);

void clear_dict();

#ifdef __cplusplus
}
#endif