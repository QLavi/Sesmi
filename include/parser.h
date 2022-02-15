#pragma once

typedef enum {
  NODE_CHAR,
  NODE_ALTER,
  NODE_CONCAT,
  NODE_QMARK,
  NODE_PLUS,
  NODE_STAR
} Node_Type;

typedef struct AST_Node AST_Node;
struct AST_Node {
  Node_Type type;
  char c;
  int id;
  AST_Node* left;
  AST_Node* right;
};

AST_Node* pattern_to_ast(char* str, size_t size);
void write_ast_in_dot(AST_Node* node);
