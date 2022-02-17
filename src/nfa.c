#include "io_and_mem.h"
#include "nfa.h"

enum {
  STATE_SPLIT = 256,
  STATE_MATCH
};

typedef struct {
  NFA_State* start;
  NFA_State** final;
} NFA_Fragment;

NFA_Fragment stack[128];
NFA_Fragment* stk_ptr = stack;
#define PUSH(x) (*stk_ptr++ = x)
#define POP() (*--stk_ptr)

int state_id = 0;
NFA_State* make_state(int c, NFA_State* s0, NFA_State* s1) {
  state_id += 1;
  NFA_State* state = ALLOC(NFA_State, 1);
  state->c = c;
  state->last_list = 0;
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
      NFA_State* state = make_state(node->c, NULL, NULL);
      state->next[0] = make_state(0, NULL, NULL);
      NFA_Fragment frag = {state, &state->next[0]};
      PUSH(frag);
    } break;
    case NODE_CONCAT:
    {
      NFA_Fragment f1 = POP();
      NFA_Fragment f0 = POP();
      *f0.final = f1.start;

      NFA_Fragment frag = {f0.start, f1.final};
      PUSH(frag);
    } break;
    case NODE_ALTER:
    {
      NFA_Fragment f1 = POP();
      NFA_Fragment f0 = POP();

      NFA_State* split = make_state(STATE_SPLIT, f0.start, f1.start);
      NFA_State* state = make_state(0, NULL, NULL);

      *f0.final = state;
      *f1.final = state;

      NFA_Fragment frag = {split, &state};
      PUSH(frag);
    } break;
    case NODE_STAR:
    {
      NFA_Fragment f0 = POP();
      NFA_State* split = make_state(STATE_SPLIT, f0.start, NULL);
      NFA_State* state = make_state(STATE_SPLIT, f0.start, NULL);
      state->next[1] = make_state(0, NULL, NULL);

      split->next[1] = state;
      *f0.final = state;

      NFA_Fragment frag = {f0.start, &state->next[1]};
      PUSH(frag);
    } break;
    case NODE_QMARK:
    {
      NFA_Fragment f0 = POP();
      NFA_State* split = make_state(STATE_SPLIT, f0.start, NULL);
      NFA_State* state = make_state(0, NULL, NULL);

      *f0.final = state;
      split->next[1] = state;

      NFA_Fragment frag = {split, &state};
      PUSH(frag);
    } break;
    case NODE_PLUS:
    {
      NFA_Fragment f0 = POP();
      *f0.final = make_state(STATE_SPLIT, f0.start, NULL);
      (*f0.final)->next[1] = make_state(0, NULL, NULL);

      NFA_Fragment frag = {f0.start, &(*f0.final)->next[1]};
      PUSH(frag);
    } break;
  }
}

NFA_State final_state = {STATE_MATCH};
NFA_State* ast_to_nfa(AST_Node* node) {
  generate_nfa(node);

  state_id += 1;
  NFA_Fragment frag = POP();
  **frag.final = final_state;
  return frag.start;
}

#if 0
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
#endif

typedef struct {
  NFA_State** states;
  int count;
}State_List;

int list_id = 0;
void add_state(State_List* list, NFA_State* state);

State_List* start_list(NFA_State* state) {
  State_List* list = ALLOC(State_List, 1);
  list->states = ALLOC(NFA_State*, state_id);
  list->count = 0;
  list_id += 1;
  add_state(list, state);
  return list;
}

void add_state(State_List* list, NFA_State* state) {
  if(state == NULL || state->last_list == list_id) return;

  state->last_list = list_id;
  if(state->c == STATE_SPLIT) {
    add_state(list, state->next[0]);
    add_state(list, state->next[1]);
    return;
  }
  list->states[list->count] = state;
  list->count += 1;
}

void nfa_step(State_List* current_states, State_List* next_states, char c) {
  list_id += 1;
  next_states->count = 0;

  for(int x = 0; x < current_states->count; x++) {
    NFA_State* state = current_states->states[x];
    if(state->c == c) {
      add_state(next_states, state->next[0]);
    }
  }
}

bool simulate_nfa(NFA_State* start, char* match) {

  State_List* current_states = start_list(start);
  State_List* next_states = start_list(NULL);

  char* p = match;
  for(;*p; p++) {
    char c = *p & 0xFF;
    nfa_step(current_states, next_states, c);

    State_List* tmp = current_states;
    current_states = next_states;
    next_states = tmp;
  }
  
  for(int x = 0; x < current_states->count; x++) {
    if(current_states->states[x]->c == STATE_MATCH) {
      return 1;
    }
  }
  return 0;
}
