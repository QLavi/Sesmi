#include "io_and_mem.h"
#include "parser.h"
#include "nfa.h"

int main(void) {
  char pattern[] = "L.a.v.i.(R|E)*.w.e*";
  size_t size = sizeof(pattern);
  AST_Node* node = pattern_to_ast(pattern, size);
  write_ast_in_dot(node);

  NFA_State* state = ast_to_nfa(node);
  write_nfa_in_dot(state);

  FREE_ALL();
  return 0;
}
