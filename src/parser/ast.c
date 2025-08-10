#include "../../include/ast.h"
#include "parser.bison.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***
 THIS IS THE IMPLEMENTATION OF THE ABSTRACT SYNTAX TREEE STRUCTURE, it very
 basic at this stage but i structure it in a way that it's very to extend yet
 still safe... hope so... nvm it's going to break:(.
 ***/

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
  node->data.expression_stmt.expression = expression;
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
AST *ast_unary_op(char op_char, AST *operand) {
  return ast_unary_op_enum(unary_op(op_char), operand);
}

AST *ast_unary_op_enum(UnaryOperator op, AST *operand) {
  AST *node = ast_create(AST_UNARY_OP);
  node->data.unary_op.unar_op = op;
  node->data.unary_op.operand = operand;
  return node;
}
AST *ast_function_call(char *name, ASTList *arguments) {
  AST *node = ast_create(AST_FUNCTION_CALL);
  node->data.function_call.name = strdup(name);
  node->data.function_call.arguments = arguments;
  return node;
}
AST *ast_identifier(char *name) {
  AST *node = ast_create(AST_IDENTIFIER);
  node->data.identifier.name = strdup(name);
  return node;
}
AST *ast_number(char *value_str) {
  AST *node = ast_create(AST_INT_LITERAL);
  node->data.int_literal.value = strtoll(value_str, NULL, 10);
  node->data.int_literal.raw_value = strdup(value_str);
  return node;
}
AST *ast_float(char *value_str) {
  AST *node = ast_create(AST_FLOAT_LITERAL);
  node->data.float_literal.value = strtod(value_str, NULL);
  node->data.float_literal.raw_value = strdup(value_str);
  return node;
}

/*List AST*/
ASTList *ast_list_create(AST *node) {
  ASTList *list = malloc(sizeof(ASTList));
  if (!list) {
    fprintf(stderr, "[PARSER]: Failed to allocate ASTList memory.");
    exit(EXIT_FAILURE);
  }
  list->node = node;
  list->next = NULL;
  return list;
}
ASTList *ast_list_append(ASTList *list, AST *node) {
  if (!list) {
    return ast_list_create(node);
  }

  ASTList *curr = list;
  while (curr->next != NULL) {
    curr = curr->next;
  }
  curr->next = ast_list_create(node);
  return list;
}
int ast_list_length(ASTList *list) {
  int len = 0;
  while (list) {
    len++;
    list = list->next;
  }
  return len;
}
void ast_list_free(ASTList *list) {
  while (list) {
    ASTList *next = list->next;
    ast_free(list->node);
    free(list);
    list = next;
  }
}

ASTList *make_function_list(AST *function, ASTList *next) {
  ASTList *list = ast_list_create(function);
  list->next = next;
  return list;
}
ASTList *append_function(ASTList *list, AST *function) {
  return ast_list_append(list, function);
}
ASTList *make_param_list(AST *param, ASTList *next) {
  ASTList *list = ast_list_create(param);
  list->next = next;
  return list;
}
ASTList *append_param(ASTList *list, AST *param) {
  return ast_list_append(list, param);
}
ASTList *make_statement_list(AST *statement, ASTList *next) {
  ASTList *list = ast_list_create(statement);
  list->next = next;
  return list;
}
ASTList *append_statement(ASTList *list, AST *statement) {
  return ast_list_append(list, statement);
}
ASTList *make_expr_list(AST *expression, ASTList *next) {
  ASTList *list = ast_list_create(expression);
  list->next = next;
  return list;
}
ASTList *append_expr(ASTList *list, AST *expression) {
  return ast_list_append(list, expression);
}

/*MEMORY MANAGEMENT*/
void type_info_free(TypeInfo *type) {
  if (type) {
    free(type->name);
    free(type);
  }
}

void ast_free(AST *node) {
  if (!node)
    return;

  switch (node->type) {
  case AST_FUNCTION_DECL:
    free(node->data.function_decl.name);
    type_info_free(node->data.function_decl.return_type);
    ast_list_free(node->data.function_decl.parameters);
    ast_free(node->data.function_decl.body);
    break;
  case AST_PARAM:
    free(node->data.param.name);
    type_info_free(node->data.param.param_type);
    break;
  case AST_TYPE:
    type_info_free(node->data.type.type_info);
    break;
  case AST_VAR_DECL:
    free(node->data.var_decl.name);
    type_info_free(node->data.var_decl.var_type);
    ast_free(node->data.var_decl.initializer);
    break;
  case AST_ASSIGNMENT:
    free(node->data.assignment.name);
    break;
  case AST_FUNCTION_CALL:
    free(node->data.function_call.name);
    break;
  case AST_IDENTIFIER:
    free(node->data.identifier.name);
    break;
  case AST_INT_LITERAL:
    free(node->data.int_literal.raw_value);
    break;
  case AST_FLOAT_LITERAL:
    free(node->data.float_literal.raw_value);
    break;
  default:
    /* No special cleanup needed for other node types */
    break;
  }

  free(node);
}

void ast_free_recursive(AST *node) {
  if (!node)
    return;

  switch (node->type) {
  case AST_PROGRAM:
    ast_list_accept(node->data.program.functions, NULL, NULL);
    ast_list_free(node->data.program.functions);
    break;
  case AST_FUNCTION_DECL:
    ast_list_accept(node->data.function_decl.parameters, NULL, NULL);
    ast_list_free(node->data.function_decl.parameters);
    ast_free_recursive(node->data.function_decl.body);
    break;
  case AST_BLOCK:
    ast_list_accept(node->data.block.statements, NULL, NULL);
    ast_list_free(node->data.block.statements);
    break;
  case AST_VAR_DECL:
    ast_free_recursive(node->data.var_decl.initializer);
    break;
  case AST_ASSIGNMENT:
    ast_free_recursive(node->data.assignment.value);
    break;
  case AST_RETURN:
    ast_free_recursive(node->data.return_stmt.expression);
    break;
  case AST_BINARY_OP:
    ast_free_recursive(node->data.binary_op.left);
    ast_free_recursive(node->data.binary_op.right);
    break;
  case AST_UNARY_OP:
    ast_free_recursive(node->data.unary_op.operand);
    break;
  case AST_FUNCTION_CALL:
    ast_list_accept(node->data.function_call.arguments, NULL, NULL);
    ast_list_free(node->data.function_call.arguments);
    break;
  default:
    /* Leaf nodes or nodes without children */
    break;
  }
  ast_free(node);
}

/*VISITOR IMPLEMENTATAION*/
void ast_accept(AST *node, ASTVisitor *visitor, void *context) {
  if (!node || !visitor)
    return;

  /* Pre-visit*/
  if (visitor->pre_visit) {
    visitor->pre_visit(node, context);
  }

  /* Type-specific visit */
  switch (node->type) {
  case AST_PROGRAM:
    if (visitor->visit_program) {
      visitor->visit_program(node, context);
    }
    /* Visit child */
    ast_list_accept(node->data.program.functions, visitor, context);
    break;

  case AST_FUNCTION_DECL:
    if (visitor->visit_function_decl) {
      visitor->visit_function_decl(node, context);
    }
    /* Visit child */
    ast_list_accept(node->data.function_decl.parameters, visitor, context);
    ast_accept(node->data.function_decl.body, visitor, context);
    break;

  case AST_PARAM:
    if (visitor->visit_param) {
      visitor->visit_param(node, context);
    }
    break;

  case AST_TYPE:
    if (visitor->visit_type) {
      visitor->visit_type(node, context);
    }
    break;

  case AST_BLOCK:
    if (visitor->visit_block) {
      visitor->visit_block(node, context);
    }
    /* Visit childe */
    ast_list_accept(node->data.block.statements, visitor, context);
    break;

  case AST_VAR_DECL:
    if (visitor->visit_var_decl) {
      visitor->visit_var_decl(node, context);
    }
    /* Visit initializer if exist */
    ast_accept(node->data.var_decl.initializer, visitor, context);
    break;

  case AST_ASSIGNMENT:
    if (visitor->visit_assignment) {
      visitor->visit_assignment(node, context);
    }
    /* Visit val*/
    ast_accept(node->data.assignment.value, visitor, context);
    break;

  case AST_RETURN:
    if (visitor->visit_return) {
      visitor->visit_return(node, context);
    }
    /* Visit expression if exist*/
    ast_accept(node->data.return_stmt.expression, visitor, context);
    break;

  case AST_BINARY_OP:
    if (visitor->visit_binary_op) {
      visitor->visit_binary_op(node, context);
    }
    /* Visit operands */
    ast_accept(node->data.binary_op.left, visitor, context);
    ast_accept(node->data.binary_op.right, visitor, context);
    break;

  case AST_UNARY_OP:
    if (visitor->visit_unary_op) {
      visitor->visit_unary_op(node, context);
    }
    /* Visit operand */
    ast_accept(node->data.unary_op.operand, visitor, context);
    break;

  case AST_FUNCTION_CALL:
    if (visitor->visit_function_call) {
      visitor->visit_function_call(node, context);
    }
    /* Visit arguments */
    ast_list_accept(node->data.function_call.arguments, visitor, context);
    break;

  case AST_IDENTIFIER:
    if (visitor->visit_identifier) {
      visitor->visit_identifier(node, context);
    }
    break;

  case AST_INT_LITERAL:
    if (visitor->visit_int_literal) {
      visitor->visit_int_literal(node, context);
    }
    break;

  case AST_FLOAT_LITERAL:
    if (visitor->visit_float_literal) {
      visitor->visit_float_literal(node, context);
    }
    break;

  default:
    /* Unknown type */
    break;
  }

  /* Post-visit*/
  if (visitor->post_visit) {
    visitor->post_visit(node, context);
  }
}

void ast_list_accept(ASTList *list, ASTVisitor *visitor, void *context) {
  while (list) {
    ast_accept(list->node, visitor, context);
    list = list->next;
  }
}

/* PRINTING UTILITIES */
char *ast_node_type_string(ASTNodeType type) {
  switch (type) {
  case AST_PROGRAM:
    return "Program";
  case AST_FUNCTION_DECL:
    return "FunctionDecl";
  case AST_FUNCTION_LIST:
    return "FunctionList";
  case AST_PARAM:
    return "Parameter";
  case AST_PARAM_LIST:
    return "ParameterList";
  case AST_TYPE:
    return "Type";
  case AST_BLOCK:
    return "Block";
  case AST_STATEMENT_LIST:
    return "StatementList";
  case AST_VAR_DECL:
    return "VarDecl";
  case AST_ASSIGNMENT:
    return "Assignment";
  case AST_RETURN:
    return "Return";
  case AST_EXPRESSION_STMT:
    return "ExpressionStmt";
  case AST_BINARY_OP:
    return "BinaryOp";
  case AST_UNARY_OP:
    return "UnaryOp";
  case AST_FUNCTION_CALL:
    return "FunctionCall";
  case AST_IDENTIFIER:
    return "Identifier";
  case AST_INT_LITERAL:
    return "IntLiteral";
  case AST_FLOAT_LITERAL:
    return "FloatLiteral";
  case AST_EXPR_LIST:
    return "ExpressionList";
  default:
    return "Unknown";
  }
}

char *binary_operator_string(BinaryOperator op) {
  switch (op) {
  case BINOP_ADD:
    return "+";
  case BINOP_SUB:
    return "-";
  case BINOP_MUL:
    return "*";
  case BINOP_DIV:
    return "/";
  case BINOP_MOD:
    return "%";
  case BINOP_EQ:
    return "==";
  case BINOP_NE:
    return "!=";
  case BINOP_LT:
    return "<";
  case BINOP_GT:
    return ">";
  case BINOP_LE:
    return "<=";
  case BINOP_GE:
    return ">=";
  default:
    return "?";
  }
}

char *unary_operator_string(UnaryOperator op) {
  switch (op) {
  case UNOP_MINUS:
    return "-";
  case UNOP_PLUS:
    return "+";
  case UNOP_NOT:
    return "!";
  default:
    return "?";
  }
}

char *type_kind_string(TypeKind kind) {
  switch (kind) {
  case TYPE_INT:
    return "int";
  case TYPE_FLOAT:
    return "float";
  case TYPE_VOID:
    return "void";
  case TYPE_CUSTOM:
    return "custom";
  default:
    return "unknown";
  }
}

static void print_indent(int indent) {
  for (int i = 0; i < indent; i++) {
    printf(" ");
  }
}

void ast_print_list(ASTList *list, int indent, const char *separator) {
  while (list) {
    ast_print(list->node, indent);
    if (list->next && separator) {
      printf("%s", separator);
    }
    list = list->next;
  }
}
// Printing the entire tree using tree-traversal
void ast_print(AST *node, int indent) {
  if (!node) {
    print_indent(indent);
    printf("(null)\n");
    return;
  }

  print_indent(indent);
  printf("%s", ast_node_type_string(node->type));

  switch (node->type) {
  case AST_PROGRAM:
    printf(" {\n");
    ast_print_list(node->data.program.functions, indent + 1, NULL);
    print_indent(indent);
    printf("}\n");
    break;

  case AST_FUNCTION_DECL:
    printf(": %s -> %s {\n", node->data.function_decl.name,
           node->data.function_decl.return_type
               ? type_kind_string(node->data.function_decl.return_type->kind)
               : "void");
    if (node->data.function_decl.parameters) {
      print_indent(indent + 1);
      printf("Parameters:\n");
      ast_print_list(node->data.function_decl.parameters, indent + 2, NULL);
    }
    if (node->data.function_decl.body) {
      print_indent(indent + 1);
      printf("Body:\n");
      ast_print(node->data.function_decl.body, indent + 2);
    }
    print_indent(indent);
    printf("}\n");
    break;

  case AST_PARAM:
    printf(": %s : %s\n", node->data.param.name,
           node->data.param.param_type
               ? type_kind_string(node->data.param.param_type->kind)
               : "unknown");
    break;

  case AST_BLOCK:
    printf(" {\n");
    if (node->data.block.statements) {
      ast_print_list(node->data.block.statements, indent + 1, NULL);
    }
    print_indent(indent);
    printf("}\n");
    break;

  case AST_VAR_DECL:
    printf(": %s : %s", node->data.var_decl.name,
           node->data.var_decl.var_type
               ? type_kind_string(node->data.var_decl.var_type->kind)
               : "unknown");
    if (node->data.var_decl.initializer) {
      printf(" = \n");
      ast_print(node->data.var_decl.initializer, indent + 1);
    } else {
      printf("\n");
    }
    break;

  case AST_ASSIGNMENT:
    printf(": %s = \n", node->data.assignment.name);
    ast_print(node->data.assignment.value, indent + 1);
    break;

  case AST_RETURN:
    if (node->data.return_stmt.expression) {
      printf(":\n");
      ast_print(node->data.return_stmt.expression, indent + 1);
    } else {
      printf(": (void)\n");
    }
    break;

  case AST_BINARY_OP:
    printf(": %s\n", binary_operator_string(node->data.binary_op.bin_op));
    print_indent(indent + 1);
    printf("Left:\n");
    ast_print(node->data.binary_op.left, indent + 2);
    print_indent(indent + 1);
    printf("Right:\n");
    ast_print(node->data.binary_op.right, indent + 2);
    break;

  case AST_UNARY_OP:
    printf(": %s\n", unary_operator_string(node->data.unary_op.unar_op));
    ast_print(node->data.unary_op.operand, indent + 1);
    break;

  case AST_FUNCTION_CALL:
    printf(": %s(", node->data.function_call.name);
    if (node->data.function_call.arguments) {
      printf("\n");
      ast_print_list(node->data.function_call.arguments, indent + 1, ",\n");
      print_indent(indent);
      printf(")\n");
    } else {
      printf(")\n");
    }
    break;

  case AST_IDENTIFIER:
    printf(": %s\n", node->data.identifier.name);
    break;

  case AST_INT_LITERAL:
    printf(": %lld (raw: %s)\n", node->data.int_literal.value,
           node->data.int_literal.raw_value ? node->data.int_literal.raw_value
                                            : "null");
    break;

  case AST_FLOAT_LITERAL:
    printf(": %f (raw: %s)\n", node->data.float_literal.value,
           node->data.float_literal.raw_value
               ? node->data.float_literal.raw_value
               : "null");
    break;

  case AST_TYPE:
    printf(": %s\n", node->data.type.type_info
                         ? type_kind_string(node->data.type.type_info->kind)
                         : "unknown");
    break;

  default:
    printf(": (unhandled node type)\n");
    break;
  }
}

/*ERrror Reportign*/
void ast_error(AST *node, const char *message, ...) {
  va_list args;
  va_start(args, message);

  fprintf(stderr, "AST Error");
  if (node && node->line >= 0) {
    fprintf(stderr, " at line %d", node->line);
    if (node->column >= 0) {
      fprintf(stderr, ", column %d", node->column);
    }
  }
  fprintf(stderr, ": ");

  vfprintf(stderr, message, args);
  fprintf(stderr, "\n");

  va_end(args);
}

void ast_warning(AST *node, const char *message, ...) {
  va_list args;
  va_start(args, message);

  fprintf(stderr, "AST Warning");
  if (node && node->line >= 0) {
    fprintf(stderr, " at line %d", node->line);
    if (node->column >= 0) {
      fprintf(stderr, ", column %d", node->column);
    }
  }
  fprintf(stderr, ": ");

  vfprintf(stderr, message, args);
  fprintf(stderr, "\n");

  va_end(args);
}
