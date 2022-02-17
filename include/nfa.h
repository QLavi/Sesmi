#pragma once

#include <stdbool.h>
#include "parser.h"

typedef struct NFA_State NFA_State;
struct NFA_State {
  int c;
  int last_list;
  NFA_State* next[2];
};

NFA_State* ast_to_nfa(AST_Node* node);
void write_nfa_in_dot(NFA_State* state);
bool simulate_nfa(NFA_State* start, char* match);
