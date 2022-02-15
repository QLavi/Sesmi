#pragma once

#include <stdbool.h>
#include "parser.h"

typedef struct NFA_State NFA_State;
struct NFA_State {
  char c[2];
  int id;
  bool reached[2];
  NFA_State* next[2];
};

NFA_State* ast_to_nfa(AST_Node* node);
void write_nfa_in_dot(NFA_State* state);
