#include "io_and_mem.h"
#include "parser.h"

typedef enum {
  TOKEN_VBAR, TOKEN_DOT,
  TOKEN_QMARK, TOKEN_PLUS,
  TOKEN_STAR, TOKEN_CHAR,
  TOKEN_LPAR, TOKEN_RPAR,
  TOKEN_EOF
} Token_Type;

typedef struct {
  Token_Type type;
  char c;
} Token;

char* ptr;
Token next_token(void) {
  if(*ptr == '\0') return (Token){.type=TOKEN_EOF, .c='\0'};
  char c = *ptr;
  ptr += 1;
  switch(c) {
    case '|': return (Token){.type=TOKEN_VBAR, .c=c};
    case '.': return (Token){.type=TOKEN_DOT,  .c=c};
    case '?': return (Token){.type=TOKEN_QMARK,.c=c};
    case '+': return (Token){.type=TOKEN_PLUS, .c=c};
    case '*': return (Token){.type=TOKEN_STAR, .c=c};
    case '(': return (Token){.type=TOKEN_LPAR, .c=c};
    case ')': return (Token){.type=TOKEN_RPAR, .c=c};
    default : return (Token){.type=TOKEN_CHAR, .c=c};
  }
}


typedef struct {
  Token prev;
  Token curr;
} Parser;

Parser parser;

void consume(Token_Type type, char* err_descr) {
  if(parser.curr.type == type) {
    parser.curr = next_token();
  }
  else {
    fprintf(stderr, "%s\n", err_descr);
  }
}

int id = 0;
AST_Node* make_node(int type, AST_Node* left, AST_Node* right) {
  AST_Node* node = ALLOC(AST_Node, 1);
  node->type = type;
  node->id = id;
  id += 1;
  node->left = left;
  node->right = right;
  return node;
}

typedef enum {
  PREC_NONE,
  PREC_ALTER,
  PREC_CONCAT,
  PREC_REPETITION
} Precedence;

typedef AST_Node* (*Infix_Fn)(AST_Node*);
typedef AST_Node* (*Prefix_Fn)(void);
typedef struct {
  Prefix_Fn prefix;
  Infix_Fn infix;
  Precedence precedence;
} Parse_Rule;

AST_Node* charc(void);
AST_Node* group(void);
AST_Node* infix(AST_Node* node);
AST_Node* postfix(AST_Node* node);

Parse_Rule rules[] = {
  [TOKEN_LPAR]  = {group, NULL, PREC_NONE},
  [TOKEN_RPAR]  = {NULL, NULL, PREC_NONE},
  [TOKEN_VBAR]  = {NULL, infix, PREC_ALTER},
  [TOKEN_DOT]   = {NULL, infix, PREC_CONCAT},
  [TOKEN_QMARK] = {NULL, postfix, PREC_REPETITION},
  [TOKEN_PLUS]  = {NULL, postfix, PREC_REPETITION},
  [TOKEN_STAR]  = {NULL, postfix, PREC_REPETITION},
  [TOKEN_CHAR]  = {charc, NULL, PREC_NONE},
  [TOKEN_EOF]   = {NULL, NULL, PREC_NONE},
};

AST_Node* expression(Precedence precedence) {
  parser.prev = parser.curr;
  parser.curr = next_token();
  AST_Node* node = rules[parser.prev.type].prefix();

  while(precedence < rules[parser.curr.type].precedence) {
    parser.prev = parser.curr;
    parser.curr = next_token();
    node = rules[parser.prev.type].infix(node);
  }
  return node;
}

AST_Node* charc(void) {
  char c = parser.prev.c;
  return make_node(c, NULL, NULL);
}
AST_Node* group(void) {
  AST_Node* node = expression(PREC_NONE);
  consume(TOKEN_RPAR, "Incomplete Set of Parentheses");
  return node;
}
AST_Node* infix(AST_Node* node) {
  switch(parser.prev.type) {
    case TOKEN_DOT:
      return make_node('c', node, expression(PREC_CONCAT));
    case TOKEN_VBAR:
      return make_node('v' , node, expression(PREC_ALTER));
  }
}
AST_Node* postfix(AST_Node* node) {
  switch(parser.prev.type) {
    case TOKEN_QMARK:
      return make_node('q', node, NULL);
    case TOKEN_PLUS:
      return make_node('p', node, NULL);
    case TOKEN_STAR:
      return make_node('s', node, NULL);
  }
}

AST_Node* pattern_to_ast(char* str, size_t size) {
  ptr = str;
  parser.curr = next_token();
  return expression(PREC_NONE);
}

#define dwrite(...) fprintf(fptr, __VA_ARGS__)
void write_node_ids(FILE* fptr, AST_Node* node) {
  if(node == NULL) return;

  if(node->left != NULL) {
    dwrite("\t%c_%i -> %c_%i\n", node->type, node->id, node->left->type, node->left->id);
  }
  if(node->right != NULL) {
    dwrite("\t%c_%i -> %c_%i\n", node->type, node->id, node->right->type, node->right->id);
  }

  write_node_ids(fptr, node->left);
  write_node_ids(fptr, node->right);
}

void write_ast_in_dot(AST_Node* node) {
  FILE* fptr = fopen("ast.dot", "w");
  dwrite("digraph AST {\n\tnode [shape=circle]\n");
  write_node_ids(fptr, node);
  dwrite("}\n");
  fclose(fptr);
}
