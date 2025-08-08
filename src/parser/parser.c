#include "../../include/ast.h"
#include "../../include/lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*STATIC & UTILITIES FUNCTION*/
static BinaryOperator binary_op(char op) {
  switch (op) {
  case '+':
    return BINOP_ADD;
  case '-':
    return BINOP_SUB;
  case '*':
    return BINOP_MUL;
  case '/':
    return BINOP_DIV;
  default:
    fprintf(stderr, "[PARSER]: Unknown operand, binary: '+', '-', '*', '/'.");
    exit(EXIT_FAILURE);
  }
}

static UnaryOperator unary_op(char op) {
  switch (op) {
  case '+':
    return UNOP_PLUS;
  case '-':
    return UNOP_MINUS;
  default:
    fprintf(stderr, "[PARSER]: Unknown operand, unary: '+', '-', '!'");
    exit(EXIT_FAILURE);
  }
}

TypeInfo *create_type(TypeKind kind, char *name) {
  TypeInfo *type = malloc(sizeof(TypeInfo));
  if (!type) {
    fprintf(stderr, "[PARSER]: failed to allocate type memory.");
    exit(EXIT_FAILURE);
  }
  type->kind = kind;
  type->name = name ? strdup(name) : NULL;
  return type;
}

/*IMPLEMENTATION OF AST*/
AST *ast_create(ASTNodeType type) {
  AST *node = calloc(1, sizeof(AST));
  if (!node) {
    fprintf(stderr, "[PARSER]: Failed to allocate memory.");
    exit(EXIT_FAILURE);
  }
  node->type = type;
  node->column = -1;
  node->line = -1;

  return node;
}
AST *ast_program(ASTList *functions) {
  AST *node = ast_create(AST_PROGRAM);
  node->data.program.functions = functions;
  return node;
}
AST *ast_function_decl(char *name, ASTList *params, TypeInfo *return_type,
                       AST *body) {
  AST *node = ast_create(AST_FUNCTION_DECL);
  node->data.function_decl.name = strdup(name);
  node->data.function_decl.parameters = params;
  node->data.function_decl.body = body;
  node->data.function_decl.return_type = return_type;
  return node;
}
AST *ast_param(char *name, TypeInfo *type) {
  AST *node = ast_create(AST_PARAM);
  node->data.param.name = strdup(name);
  node->data.param.param_type = type;
  return node;
}
AST *ast_type(char *type_name) {
  AST *node = ast_create(AST_TYPE);
  node->data.type.type_info = ast_type_custom(type_name);
  return node;
}

/*TYPE INFORMATION*/
TypeInfo *ast_type_int(void) { return create_type(TYPE_INT, "int"); }
TypeInfo *ast_type_float(void) { return create_type(TYPE_FLOAT, "float"); }
TypeInfo *ast_type_void(void) { return create_type(TYPE_VOID, "void"); }
TypeInfo *ast_type_custom(char *name) { return create_type(TYPE_CUSTOM, name); }

/*AST IMPLEMENTATION*/
AST *ast_block(ASTList *statements) {
  AST *node = ast_create(AST_BLOCK);
  node->data.block.statements = statements;
  return node;
}

AST *ast_var_decl(char *name, TypeInfo *type, AST *initializer) {
  AST *node = ast_create(AST_VAR_DECL);
  node->data.var_decl.name = strdup(name);
  node->data.var_decl.var_type = type;
  node->data.var_decl.initializer = initializer;
  return node;
}
AST *ast_assignment(char *name, AST *value) {
  AST *node = ast_create(AST_ASSIGNMENT);
  node->data.assignment.name = strdup(name);
  node->data.assignment.value = value;
  return node;
}
AST *ast_return(AST *expression) {
  AST *node = ast_create(AST_RETURN);
  node->data.return_stmt.expression = expression;
  return node;
}
AST *ast_expression_stmt(AST *expression) {
  AST *node = ast_create(AST_EXPRESSION_STMT);
  return node;
}
AST *ast_binary_op(char op_char, AST *left, AST *right) {
  return ast_binary_op_enum(binary_op(op_char), left, right);
}
AST *ast_binary_op_enum(BinaryOperator op, AST *left, AST *right) {
  AST *node = ast_create(AST_BINARY_OP);
  node->data.binary_op.bin_op = op;
  node->data.binary_op.left = left;
  node->data.binary_op.right = right;
  return node;
}
AST *ast_unary_op(char op_char, AST *operand){
  return ast_unary_op_enum(unary_op(op_char), operand);
}

AST *ast_unary_op_enum(UnaryOperator op, AST *operand){
  AST *node = ast_create(AST_UNARY_OP);
  node->data.unary_op.unar_op = op;
  node->data.unary_op.operand = operand;
  return node;
}
AST *ast_function_call(char *name, ASTList *arguments){
  AST *node = ast_create(AST_FUNCTION_CALL);
  node->data.function_call.name = strdup(name);
  node->data.function_call.arguments = arguments;
  return node;
}
AST *ast_identifier(char *name){
  AST *node = ast_create(AST_IDENTIFIER);
  node->data.identifier.name = strdup(name);
  return node;
}
AST *ast_number(char *value_str){
  AST *node = ast_create(AST_INT_LITERAL);
  node->data.int_literal.value = strtoll(value_str, NULL, 10);
  node->data.int_literal.raw_value = strdup(value_str);
  return node;
}
AST *ast_float(char *value_str){
  AST *node = ast_create(AST_FLOAT_LITERAL);
  node->data.float_literal.value = strtod(value_str, NULL);
  node->data.float_literal.raw_value = strdup(value_str);
  return node;
}
