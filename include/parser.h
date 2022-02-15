#pragma once

typedef struct AST_Node AST_Node;
struct AST_Node {
  int type;
  int id;
  AST_Node* left;
  AST_Node* right;
};

AST_Node* pattern_to_ast(char* str, size_t size);
void write_ast_in_dot(AST_Node* node);
