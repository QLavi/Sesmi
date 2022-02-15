#include "io_and_mem.h"
#include "nfa.h"

typedef struct {
  NFA_State* start;
  NFA_State* final;
} NFA_Fragment;

NFA_Fragment stack[128];
NFA_Fragment* stk_ptr = stack;
#define PUSH(x) (*stk_ptr++ = x)
#define POP() (*--stk_ptr)

int state_id = 0;
NFA_State* make_state(char c0, char c1, NFA_State* s0, NFA_State* s1) {
  NFA_State* state = ALLOC(NFA_State, 1);
  state->c[0] = c0;
  state->c[1] = c1;
  state->id = state_id;
  state_id += 1;
  state->reached[0] = false;
  state->reached[1] = false;
  state->next[0] = s0;
  state->next[1] = s1;
  return state;
}

void generate_nfa(AST_Node* node) {
  if(node == NULL) return;

  generate_nfa(node->left);
  generate_nfa(node->right);

  switch(node->type) {
    case NODE_CHAR:
    {
      NFA_State* start = make_state(node->c, 'e', NULL, NULL);
      NFA_State* final = make_state('e', 'e', NULL, NULL);
      start->next[0] = final;

      NFA_Fragment frag = {start, final};
      PUSH(frag);
    } break;
    case NODE_CONCAT:
    {
      NFA_Fragment f1 = POP();
      NFA_Fragment f0 = POP();

      f0.final->next[0] = f1.start;

      NFA_Fragment frag = {f0.start, f1.final};
      PUSH(frag);
    } break;
    case NODE_ALTER:
    {
      NFA_Fragment f1 = POP();
      NFA_Fragment f0 = POP();

      NFA_State* start = make_state('e', 'e', f0.start, f1.start);
      NFA_State* final = make_state('e', 'e', NULL, NULL);

      f0.final->next[0] = final;
      f1.final->next[0] = final;

      NFA_Fragment frag = {start, final};
      PUSH(frag);
    } break;
    case NODE_QMARK:
    {
      NFA_Fragment f0 = POP();

      NFA_State* start = make_state('e', 'e', f0.start, NULL);
      NFA_State* final = make_state('e', 'e', NULL, NULL);
      f0.final->next[0] = final;
      start->next[1] = final;

      NFA_Fragment frag = {start, final};
      PUSH(frag);
    } break;
    case NODE_PLUS:
    {
      NFA_Fragment f0 = POP();

      NFA_State* state = make_state('e', 'e', f0.start, NULL);
      f0.final->next[0] = f0.start;
      f0.final->next[1] = make_state('e', 'e', NULL, NULL);

      NFA_Fragment frag = {state, f0.final->next[1]};
      PUSH(frag);
    } break;
    case NODE_STAR:
    {
      NFA_Fragment f0 = POP();

      NFA_State* state = make_state('e', 'e', f0.start, NULL);
      NFA_State* final = make_state('e', 'e', NULL, NULL);
      f0.final->next[0] = f0.start;
      f0.final->next[1] = final;
      state->next[1] = final;

      NFA_Fragment frag = {state, final};
      PUSH(frag);
    } break;
  }
}

NFA_State* ast_to_nfa(AST_Node* node) {
  generate_nfa(node);
  NFA_Fragment frag = POP();
  return frag.start;
}

#define dwrite(...) fprintf(fptr, __VA_ARGS__)
void write_state_ids(FILE* fptr, NFA_State* state) {
  if(state == NULL) return;

  if(state->next[0] && !state->reached[0]) {
    dwrite("\t%i -> %i [label=\"%c\"]\n", state->id, state->next[0]->id, state->c[0]);
    state->reached[0] = true;
    write_state_ids(fptr, state->next[0]);
  }
  if(state->next[1] && !state->reached[1]) {
    dwrite("\t%i -> %i [label=\"%c\"]\n", state->id, state->next[1]->id, state->c[0]);
    state->reached[1] = true;
    write_state_ids(fptr, state->next[1]);
  }
}

void write_nfa_in_dot(NFA_State* state) {
  FILE* fptr = fopen("nfa.dot", "w");
  dwrite("digraph NFA {\n\trankdir=LR;\n\tnode [shape=circle];\n");
  write_state_ids(fptr, state);
  dwrite("}\n");
  fclose(fptr);
}
