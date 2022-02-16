#include "io_and_mem.h"
#include "parser.h"
#include "nfa.h"

int main(int argc, char** argv) {
  if(argc != 2) {
    fprintf(stderr, "Usage: ./play <regex> <string_to_match>\n");
    exit(1);
  }
  char* string = argv[2];
  size_t size = strlen(argv[1]);
  AST_Node* node = pattern_to_ast(pattern, size);
  write_ast_in_dot(node);

  NFA_State* state = ast_to_nfa(node);
  write_nfa_in_dot(state);

  FREE_ALL();
  return 0;
}
